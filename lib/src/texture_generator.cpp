#include "texture_generator.h"

#include <CImg.h>
#include <igl/barycentric_coordinates.h>

#include <iostream>

namespace procrock {
std::shared_ptr<Mesh> TextureGenerator::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = generate(before);
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}

// Texture Generators can be neither moved nor removed from the pipeline
bool TextureGenerator::isMoveable() const { return false; }
bool TextureGenerator::isRemovable() const { return false; }

Configuration TextureGenerator::getBaseConfiguration() {
  Configuration result;
  normalsGenerator.addOwnGroups(result, "Normals");
  roughnessGenerator.addOwnGroups(result, "Roughness");
  metalnessGenerator.addOwnGroups(result, "Metalness");
  ambientOccGenerator.addOwnGroups(result, "Ambient Occlusion");
  return result;
}

void TextureGenerator::fillTexture(TextureGroup& texGroup,
                                   std::function<Eigen::Vector3i(Eigen::Vector3d)> colorFunction) {
  int index = 0;
  auto& dataToFill = texGroup.albedoData;
  dataToFill.resize(3 * texGroup.width * texGroup.height);
  std::fill(dataToFill.begin(), dataToFill.end(), 255);
  for (const auto& pixel : texGroup.worldMap) {
    int finalValue = 0;
    Eigen::Vector3i acc{0, 0, 0};
    for (const auto& pos : pixel.positions) {
      acc += colorFunction(pos);
    }

    if (pixel.positions.size() != 0) {
      acc /= pixel.positions.size();
      dataToFill[(3 * index)] = acc.x();
      dataToFill[(3 * index) + 1] = acc.y();
      dataToFill[(3 * index) + 2] = acc.z();
    }
    index++;
  }

  normalsGenerator.modify(texGroup);
  roughnessGenerator.modify(texGroup);
  metalnessGenerator.modify(texGroup);
  ambientOccGenerator.modify(texGroup);
}
}  // namespace procrock