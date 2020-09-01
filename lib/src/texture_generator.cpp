#include "texture_generator.h"

#include <igl/barycentric_coordinates.h>

#include <chrono>
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
}  // namespace procrock