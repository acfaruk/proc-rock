#include "texture_generator.h"

#include <CImg.h>
#include <igl/barycentric_coordinates.h>

#include <iostream>

namespace procrock {
std::shared_ptr<Mesh> TextureGenerator::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = generate(before);
    calculateNormals(mesh->textures);
    calculateRoughness(mesh->textures);
    calculateMetal(mesh->textures);
    calculateAmbientOcc(mesh->textures);
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}

// Texture Generators can be neither moved nor removed from the pipeline
bool TextureGenerator::isMoveable() const { return false; }
bool TextureGenerator::isRemovable() const { return false; }

Configuration TextureGenerator::getBaseConfiguration() {
  Configuration::ConfigurationGroup baseGroup;
  baseGroup.entry = {"Base Settings", "Change General Texture generation settings"};
  baseGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Normal Strength", "How \"deep / high\" the normals should be."},
      &normalStrength,
      1.0f,
      1.9f});
  Configuration result;
  result.configGroups["Base Texture"].push_back(baseGroup);
  return result;
}

void TextureGenerator::fillTexture(const TextureGroup& texGroup,
                                   std::vector<unsigned char>& dataToFill,
                                   std::function<Eigen::Vector3i(Eigen::Vector3d)> colorFunction) {
  int index = 0;
  dataToFill.resize(3 * texGroup.width * texGroup.height);
  std::fill(dataToFill.begin(), dataToFill.end(), 255);
  for (const auto& pixel : texGroup.worldMap) {
    int finalValue = 0;
    Eigen::Vector3i acc{0, 0, 0};
    for (const auto& pos : pixel) {
      acc += colorFunction(pos);
    }

    if (pixel.size() != 0) {
      acc /= pixel.size();
      dataToFill[(3 * index)] = acc.x();
      dataToFill[(3 * index) + 1] = acc.y();
      dataToFill[(3 * index) + 2] = acc.z();
    }
    index++;
  }
}
void TextureGenerator::calculateNormals(TextureGroup& texGroup) {
  using namespace cimg_library;

  auto& data = texGroup.normalData;

  data.clear();
  data.resize(texGroup.width * texGroup.height * 3);

  CImg<unsigned char> image(texGroup.albedoData.data(), 3, texGroup.width, texGroup.height);
  image.permute_axes("YZCX");
  auto gradients = image.get_gradient("xy", 2);
  for (int i = 0; i < texGroup.width * texGroup.height; i++) {
    Eigen::Vector3f normal;
    normal.x() = (gradients[0](i) + 1024) / 2048.0;
    normal.y() = (gradients[1](i) + 1024) / 2048.0;
    normal.z() = 1 / normalStrength;
    normal = 255 * normal;

    data[(3 * i)] = (int)normal.x();
    data[(3 * i) + 1] = (int)normal.y();
    data[(3 * i) + 2] = (int)normal.z();
  }
}
void TextureGenerator::calculateRoughness(TextureGroup& texGroup) {
  using namespace cimg_library;

  auto& data = texGroup.roughnessData;

  data.clear();
  data.resize(texGroup.width * texGroup.height);

  CImg<unsigned char> image(texGroup.albedoData.data(), 3, texGroup.width, texGroup.height);
  image.permute_axes("YZCX");

  for (int i = 0; i < texGroup.width * texGroup.height; i++) {
    int value = 0;
    int y = i / texGroup.width;
    int x = i % texGroup.width;
    value = 0.2989 * (float)image(x, y, 0, 0) + 0.5970 * (float)image(x, y, 0, 1) +
            0.1140 * (float)image(x, y, 0, 2);

    data[i] = std::min(255, value);
  }
}
void TextureGenerator::calculateMetal(TextureGroup& texGroup) {
  using namespace cimg_library;

  auto& data = texGroup.metalData;

  data.clear();
  data.resize(texGroup.width * texGroup.height);

  for (int i = 0; i < texGroup.width * texGroup.height; i++) {
    data[i] = texGroup.roughnessData[i] < 10 ? 255 : 0;
  }
}
void TextureGenerator::calculateAmbientOcc(TextureGroup& texGroup) {
  using namespace cimg_library;

  auto& data = texGroup.ambientOccData;

  data.clear();
  data.resize(texGroup.width * texGroup.height);

  for (int i = 0; i < texGroup.width * texGroup.height; i++) {
    data[i] = texGroup.roughnessData[i];
  }
}
}  // namespace procrock