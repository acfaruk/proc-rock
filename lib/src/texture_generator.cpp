#include "texture_generator.h"

#include <CImg.h>
#include <igl/barycentric_coordinates.h>

#include <iostream>

namespace procrock {
std::shared_ptr<Mesh> TextureGenerator::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = generate(before);
    calculateNormals(mesh->textures, mesh->textures.normalData);
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
  result.configGroups.push_back(baseGroup);
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
void TextureGenerator::calculateNormals(const TextureGroup& texGroup,
                                        std::vector<unsigned char>& dataToFill) {
  using namespace cimg_library;

  dataToFill.clear();
  dataToFill.resize(texGroup.width * texGroup.height * 3);

  CImg<unsigned char> image(texGroup.albedoData.data(), 3, texGroup.width, texGroup.height);
  image.permute_axes("YZCX");
  auto gradients = image.get_gradient("xy", 2);
  for (int i = 0; i < texGroup.width * texGroup.height; i++) {
    Eigen::Vector3f normal;
    normal.x() = (gradients[0](i) + 1024) / 2048.0;
    normal.y() = (gradients[1](i) + 1024) / 2048.0;
    normal.z() = 1 / normalStrength;
    normal = 255 * normal;

    dataToFill[(3 * i)] = (int)normal.x();
    dataToFill[(3 * i) + 1] = (int)normal.y();
    dataToFill[(3 * i) + 2] = (int)normal.z();
  }
}
}  // namespace procrock