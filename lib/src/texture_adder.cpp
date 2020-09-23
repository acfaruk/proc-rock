#include "texture_adder.h"

#include <CImg.h>
#include <igl/barycentric_coordinates.h>

#include <iostream>

namespace procrock {
std::shared_ptr<Mesh> TextureAdder::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = generate(before);
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}

bool TextureAdder::isMoveable() const { return true; }
bool TextureAdder::isRemovable() const { return true; }

Configuration TextureAdder::getBaseConfiguration() {
  Configuration result;

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

  result.insertToConfigGroups("Proportions", proportionGroup);

  normalsGenerator.addOwnGroups(result, "Normals");
  roughnessGenerator.addOwnGroups(result, "Roughness");
  metalnessGenerator.addOwnGroups(result, "Metalness");
  ambientOccGenerator.addOwnGroups(result, "Ambient Occlusion");

  return result;
}

void TextureAdder::addTexture(TextureGroup& texGroup,
                              std::function<Eigen::Vector4i(Eigen::Vector3d)> colorFunction) {
  int index = 0;
  TextureGroup addGroup;
  addGroup.albedoChannels = 4;
  addGroup.width = texGroup.width;
  addGroup.height = texGroup.height;

  auto& addTexture = addGroup.albedoData;

  addTexture.resize(addGroup.albedoChannels * addGroup.width * addGroup.height);
  std::fill(addTexture.begin(), addTexture.end(), 0);
  for (const auto& pixel : texGroup.worldMap) {
    Eigen::Vector4i acc{0, 0, 0, 0};
    for (const auto& pos : pixel) {
      acc += colorFunction(pos);
    }

    if (pixel.size() != 0) {
      acc /= pixel.size();
      addTexture[(4 * index)] = acc.x();
      addTexture[(4 * index) + 1] = acc.y();
      addTexture[(4 * index) + 2] = acc.z();
      addTexture[(4 * index) + 3] = acc.w();
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