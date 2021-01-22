#include "texture_adder.h"

#include <CImg.h>
#include <igl/barycentric_coordinates.h>

#include <Eigen/Eigen>
#include <thread>

#include "utils/vector.h"

namespace procrock {
TextureAdder::TextureAdder(bool hideConfigurables) {
  if (hideConfigurables) return;

  Configuration::ConfigurationGroup proportionGroup;
  proportionGroup.entry = {"Channel Proportions", "Decide how much a channel affects the texture."};
  proportionGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Displacement", "How much the displacement is affected."}, &displacementProportion, 0, 1});
  proportionGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Normals", "How much the normals are affected."}, &normalProportion, 0, 1});
  proportionGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Roughness", "How much the roughness is affected."}, &roughnessProportion, 0, 1});
  proportionGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Metal", "How much the metalness is affected."}, &metalProportion, 0, 1});
  proportionGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Ambient Occ.", "How much the ambient occulsion is affected."},
      &ambientOccProportion,
      0,
      1});

  config.insertToConfigGroups("Proportions", proportionGroup);

  Configuration::ConfigurationGroup preferGroup;

  preferGroup.entry = {"Prefer specific normal directions.",
                       "Add the texture more prominently in specific normal directions."};
  preferGroup.bools.emplace_back(Configuration::SimpleEntry<bool>{
      {"Use preferred normal directions.", "Enable / Disable the feature."}, &preferred.enabled});

  std::function<bool()> preferDirRelevant = [&]() { return preferred.enabled; };
  preferGroup.float3s.emplace_back(Configuration::BoundedEntry<Eigen::Vector3f>{
      {"Preferred Normal Direction", "The direction to prefer.", preferDirRelevant},
      &preferred.direction,
      {-1, -1, -1},
      {1, 1, 1}});
  preferGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Strength", "How strong the preference is.", preferDirRelevant}, &preferred.strength, 0, 1});
  config.insertToConfigGroups("Preferred Normal Direction", preferGroup);
}

std::shared_ptr<Mesh> TextureAdder::run(Mesh* before) {
  if (isDisabled()) {
    mesh = std::make_shared<Mesh>(*before);
  } else if (isChanged() || firstRun) {
    mesh = generate(before);
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}

bool TextureAdder::isMoveable() const { return true; }
bool TextureAdder::isRemovable() const { return true; }

TextureGroup TextureAdder::createAddTexture(Mesh& mesh, TextureFunction texFunction) {
  const auto& texGroup = mesh.textures;

  TextureGroup addGroup;
  addGroup.albedoChannels = 4;
  addGroup.width = texGroup.width;
  addGroup.height = texGroup.height;

  auto& addTexture = addGroup.displacementData;

  addTexture.resize(addGroup.width * addGroup.height);
  std::fill(addTexture.begin(), addTexture.end(), 0);

  const auto threadCount = std::thread::hardware_concurrency();
  int batchCount = addTexture.size() / threadCount;

  std::vector<std::thread> threads;
  auto data = utils::splitVector(addTexture, batchCount);

  int sizeAcc = 0;
  for (int i = 0; i < data.size(); i++) {
    if (i != 0) {
      sizeAcc += data[i - 1].size();
    }
    int startIndex = sizeAcc;
    threads.emplace_back(std::thread(&TextureAdder::fillPart, std::ref(data[i]), startIndex,
                                     startIndex + data[i].size(), std::cref(mesh), texFunction,
                                     preferred));
  }

  addTexture.clear();
  for (int i = 0; i < data.size(); i++) {
    threads[i].join();
    addTexture.insert(addTexture.end(), data[i].begin(), data[i].end());
  }
  return addGroup;
}

void TextureAdder::addTextures(Mesh& mesh, TextureGroup& addGroup) {
  auto& addTexture = addGroup.albedoData;
  auto& texGroup = mesh.textures;

  for (int i = 0; i < addTexture.size() / 4; i++) {
    if (preferred.enabled) {
      auto& pixel = mesh.textures.worldMap[i];

      if (pixel.face == -1) continue;  // skip pixels on non faces..

      Eigen::Vector3i textureNormalSample = {texGroup.normalData[(3 * i)],
                                             texGroup.normalData[(3 * i) + 1],
                                             texGroup.normalData[(3 * i) + 2]};
      Eigen::Vector3f textureNormal = (textureNormalSample.cast<float>() / 255) * 2.0;
      textureNormal = textureNormal.array() - 1;
      Eigen::Vector3f faceTangent = mesh.faceTangents.row(pixel.face).cast<float>().normalized();
      Eigen::Vector3f faceNormal = mesh.faceNormals.row(pixel.face).cast<float>().normalized();

      Eigen::DiagonalMatrix<float, 3> diagMatrix(1, 1, 1);
      Eigen::Matrix3f normalMatrix = diagMatrix.toDenseMatrix().inverse();

      Eigen::Vector3f T = normalMatrix * faceTangent;
      Eigen::Vector3f N = normalMatrix * faceNormal;
      Eigen::Vector3f B = N.cross(T);

      Eigen::Matrix3f TBN;
      TBN.col(0) = T.normalized();
      TBN.col(1) = B.normalized();
      TBN.col(2) = N.normalized();

      Eigen::Vector3f normal = (TBN * textureNormal.normalized()).normalized();

      Eigen::Vector3f distance = normal - preferred.direction.normalized();
      float norm = distance.norm();
      float prefer = norm * preferred.strength;
      int value = std::min(255.0f, addTexture[(4 * i) + 3] / prefer);
      addTexture[(4 * i) + 3] = value;
    }

    float alpha = addTexture[(4 * i) + 3] / 255.0f;
    texGroup.albedoData[(3 * i)] =
        texGroup.albedoData[(3 * i)] * (1.0f - alpha) + addTexture[(4 * i)] * alpha;
    texGroup.albedoData[(3 * i) + 1] =
        texGroup.albedoData[(3 * i) + 1] * (1.0f - alpha) + addTexture[(4 * i) + 1] * alpha;
    texGroup.albedoData[(3 * i) + 2] =
        texGroup.albedoData[(3 * i) + 2] * (1.0f - alpha) + addTexture[(4 * i) + 2] * alpha;

    if (!addGroup.displacementData.empty()) {
      float displacementAlpha = displacementProportion * alpha;
      texGroup.displacementData[i] = texGroup.displacementData[i] * (1.0f - displacementAlpha) +
                                     addGroup.displacementData[i] * displacementAlpha;
    }

    if (!addGroup.normalData.empty()) {
      float normalAlpha = normalProportion * alpha;
      texGroup.normalData[(3 * i) + 0] = texGroup.normalData[(3 * i) + 0] * (1.0f - normalAlpha) +
                                         addGroup.normalData[(3 * i) + 0] * normalAlpha;
      texGroup.normalData[(3 * i) + 1] = texGroup.normalData[(3 * i) + 1] * (1.0f - normalAlpha) +
                                         addGroup.normalData[(3 * i) + 1] * normalAlpha;
      texGroup.normalData[(3 * i) + 2] = texGroup.normalData[(3 * i) + 2] * (1.0f - normalAlpha) +
                                         addGroup.normalData[(3 * i) + 2] * normalAlpha;
    }

    if (!addGroup.roughnessData.empty()) {
      float roughnessAlpha = roughnessProportion * alpha;
      texGroup.roughnessData[i] = texGroup.roughnessData[i] * (1.0f - roughnessAlpha) +
                                  addGroup.roughnessData[i] * roughnessAlpha;
    }

    if (!addGroup.metalData.empty()) {
      float metalAlpha = metalProportion * alpha;
      texGroup.metalData[i] =
          texGroup.metalData[i] * (1.0f - metalAlpha) + addGroup.metalData[i] * metalAlpha;
    }

    if (!addGroup.ambientOccData.empty()) {
      float ambientOccAlpha = ambientOccProportion * alpha;
      texGroup.ambientOccData[i] = texGroup.ambientOccData[i] * (1.0f - ambientOccAlpha) +
                                   addGroup.ambientOccData[i] * ambientOccAlpha;
    }
  }
}
void TextureAdder::fillPart(std::vector<float>& data, int startIndex, int endIndex,
                            const Mesh& mesh, TextureFunction texFunction,
                            PreferredNormalDirectionStruct preferred) {
  const auto& texGroup = mesh.textures;

  for (int i = startIndex; i < endIndex; i++) {
    const auto& pixel = texGroup.worldMap[i];

    float acc = 0;
    for (const auto& pos : pixel.positions) {
      acc += texFunction(pos);
    }

    int index = i - startIndex;
    acc /= pixel.positions.size();
    data[index] = acc;
  }
}
}  // namespace procrock