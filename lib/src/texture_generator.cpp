#include "texture_generator.h"

#include <CImg.h>
#include <igl/barycentric_coordinates.h>

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
  auto gradients = image.get_gradient(0, 2);
  for (int i = 0; i < texGroup.width * texGroup.height; i++) {
    Eigen::Vector3f normal;
    normal.x() = gradients[0].data()[i];
    normal.y() = gradients[1].data()[i];
    normal.z() = 1.0 / 2;
    normal = normal.normalized();
    normal = 255 * normal;

    dataToFill[(3 * i)] = (int)normal.x();
    dataToFill[(3 * i) + 1] = (int)normal.y();
    dataToFill[(3 * i) + 2] = (int)normal.z();
  }
}
}  // namespace procrock