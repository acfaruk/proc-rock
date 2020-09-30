#include "texture_adder.h"

#include <CImg.h>
#include <igl/barycentric_coordinates.h>

#include <iostream>

namespace procrock {
TextureAdder::TextureAdder() {
  Configuration::ConfigurationGroup proportionGroup;
  proportionGroup.entry = {"Channel Proportions", "Decide how much a channel affects the texture."};
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
      {"Use preferred normal directions.", "Enable / Disable the feature."},
      &usePreferredNormalDirection});

  std::function<bool()> preferDirRelevant = [&]() { return usePreferredNormalDirection; };
  preferGroup.float3s.emplace_back(Configuration::BoundedEntry<Eigen::Vector3f>{
      {"Preferred Normal Direction", "The direction to prefer.", preferDirRelevant},
      &preferredNormalDirection,
      {-1, -1, -1},
      {1, 1, 1}});
  preferGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Strength", "How strong the preference is.", preferDirRelevant},
      &preferredNormalStrength,
      0,
      1});
  config.insertToConfigGroups("Preferred Normal Direction", preferGroup);

  normalsGenerator.addOwnGroups(config, "Normals");
  roughnessGenerator.addOwnGroups(config, "Roughness");
  metalnessGenerator.addOwnGroups(config, "Metalness");
  ambientOccGenerator.addOwnGroups(config, "Ambient Occlusion");
}

std::shared_ptr<Mesh> TextureAdder::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = generate(before);
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}

bool TextureAdder::isMoveable() const { return true; }
bool TextureAdder::isRemovable() const { return true; }

void TextureAdder::addTexture(Mesh& mesh,
                              std::function<Eigen::Vector4i(Eigen::Vector3d)> colorFunction) {
  int index = 0;

  auto& texGroup = mesh.textures;

  TextureGroup addGroup;
  addGroup.albedoChannels = 4;
  addGroup.width = texGroup.width;
  addGroup.height = texGroup.height;

  auto& addTexture = addGroup.albedoData;

  addTexture.resize(addGroup.albedoChannels * addGroup.width * addGroup.height);
  std::fill(addTexture.begin(), addTexture.end(), 0);
  for (const auto& pixel : texGroup.worldMap) {
    Eigen::Vector4i acc{0, 0, 0, 0};
    for (const auto& pos : pixel.positions) {
      acc += colorFunction(pos);
    }

    if (pixel.positions.size() != 0) {
      acc /= pixel.positions.size();
      addTexture[(4 * index)] = acc.x();
      addTexture[(4 * index) + 1] = acc.y();
      addTexture[(4 * index) + 2] = acc.z();

      if (usePreferredNormalDirection) {
        Eigen::Vector3i textureNormalSample = {texGroup.normalData[(3 * index)],
                                               texGroup.normalData[(3 * index) + 1],
                                               texGroup.normalData[(3 * index) + 2]};
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

        Eigen::Vector3f distance = normal - preferredNormalDirection.normalized();
        float norm = distance.norm();
        float prefer = norm * preferredNormalStrength;
        int value = std::min(255.0f, acc.w() / prefer);
        addTexture[(4 * index) + 3] = value;
      } else {
        addTexture[(4 * index) + 3] = acc.w();
      }
    }
    index++;
  }

  normalsGenerator.modify(addGroup);
  roughnessGenerator.modify(addGroup);
  metalnessGenerator.modify(addGroup);
  ambientOccGenerator.modify(addGroup);

  for (int i = 0; i < addTexture.size() / 4; i++) {
    float alpha = addTexture[(4 * i) + 3] / 255.0f;
    texGroup.albedoData[(3 * i)] =
        texGroup.albedoData[(3 * i)] * (1.0f - alpha) + addTexture[(4 * i)] * alpha;
    texGroup.albedoData[(3 * i) + 1] =
        texGroup.albedoData[(3 * i) + 1] * (1.0f - alpha) + addTexture[(4 * i) + 1] * alpha;
    texGroup.albedoData[(3 * i) + 2] =
        texGroup.albedoData[(3 * i) + 2] * (1.0f - alpha) + addTexture[(4 * i) + 2] * alpha;

    float normalAlpha = normalProportion * alpha;
    texGroup.normalData[(3 * i) + 0] = texGroup.normalData[(3 * i) + 0] * (1.0f - normalAlpha) +
                                       addGroup.normalData[(3 * i) + 0] * normalAlpha;
    texGroup.normalData[(3 * i) + 1] = texGroup.normalData[(3 * i) + 1] * (1.0f - normalAlpha) +
                                       addGroup.normalData[(3 * i) + 1] * normalAlpha;
    texGroup.normalData[(3 * i) + 2] = texGroup.normalData[(3 * i) + 2] * (1.0f - normalAlpha) +
                                       addGroup.normalData[(3 * i) + 2] * normalAlpha;

    float roughnessAlpha = roughnessProportion * alpha;
    texGroup.roughnessData[i + 0] = texGroup.roughnessData[i + 0] * (1.0f - roughnessAlpha) +
                                    addGroup.roughnessData[i] * roughnessAlpha;
    texGroup.roughnessData[i + 1] = texGroup.roughnessData[i + 1] * (1.0f - roughnessAlpha) +
                                    addGroup.roughnessData[i + 1] * roughnessAlpha;
    texGroup.roughnessData[i + 2] = texGroup.roughnessData[i + 2] * (1.0f - roughnessAlpha) +
                                    addGroup.roughnessData[i + 2] * roughnessAlpha;

    float metalAlpha = metalProportion * alpha;
    texGroup.metalData[i + 0] =
        texGroup.metalData[i] * (1.0f - metalAlpha) + addGroup.metalData[i + 0] * metalAlpha;
    texGroup.metalData[i] * (1.0f - metalAlpha) + addGroup.metalData[i + 0] * metalAlpha;
    texGroup.metalData[i + 1] =
        texGroup.metalData[i + 1] * (1.0f - metalAlpha) + addGroup.metalData[i + 1] * metalAlpha;
    texGroup.metalData[i + 2] =
        texGroup.metalData[i + 2] * (1.0f - metalAlpha) + addGroup.metalData[i + 2] * metalAlpha;

    float ambientOccAlpha = ambientOccProportion * alpha;
    texGroup.ambientOccData[i + 0] = texGroup.ambientOccData[i + 0] * (1.0f - ambientOccAlpha) +
                                     addGroup.ambientOccData[i + 0] * ambientOccAlpha;
    texGroup.ambientOccData[i + 1] = texGroup.ambientOccData[i + 1] * (1.0f - ambientOccAlpha) +
                                     addGroup.ambientOccData[i + 1] * ambientOccAlpha;
    texGroup.ambientOccData[i + 2] = texGroup.ambientOccData[i + 2] * (1.0f - ambientOccAlpha) +
                                     addGroup.ambientOccData[i + 2] * ambientOccAlpha;
  }
}
}  // namespace procrock