#include "texture_generator.h"

#include <CImg.h>
#include <igl/barycentric_coordinates.h>

#include <thread>

#include "utils/vector.h"

namespace procrock {
TextureGenerator::TextureGenerator() {}

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

void TextureGenerator::fillTexture(TextureGroup& texGroup,
                                   std::function<Eigen::Vector3i(Eigen::Vector3d)> colorFunction) {
  int index = 0;
  auto& dataToFill = texGroup.albedoData;
  dataToFill.resize(3 * texGroup.width * texGroup.height);
  std::fill(dataToFill.begin(), dataToFill.end(), 0);

  const auto threadCount = std::thread::hardware_concurrency();
  int batchCount = dataToFill.size() / threadCount;
  while (batchCount % 3 != 0) batchCount++;

  std::vector<std::thread> threads;
  auto data = utils::splitVector(dataToFill, batchCount);

  int sizeAcc = 0;
  for (int i = 0; i < data.size(); i++) {
    if (i != 0) {
      sizeAcc += data[i - 1].size();
    }
    int startIndex = sizeAcc;
    threads.emplace_back(std::thread(&TextureGenerator::fillPart, std::ref(data[i]), startIndex,
                                     startIndex + data[i].size(), std::cref(texGroup.worldMap),
                                     colorFunction));
  }

  dataToFill.clear();
  for (int i = 0; i < data.size(); i++) {
    threads[i].join();
    dataToFill.insert(dataToFill.end(), data[i].begin(), data[i].end());
  }
}

void TextureGenerator::fillPart(std::vector<unsigned char>& data, int startIndex, int endIndex,
                                const std::vector<TextureGroup::WorldMapEntry>& entries,
                                std::function<Eigen::Vector3i(Eigen::Vector3d)> colorFunction) {
  for (int i = startIndex; i < endIndex; i += 3) {
    const auto& pixel = entries[i / 3];

    Eigen::Vector3i acc{0, 0, 0};
    for (const auto& pos : pixel.positions) {
      acc += colorFunction(pos);
    }

    int index = i - startIndex;
    if (pixel.positions.size() != 0) {
      acc /= pixel.positions.size();
      data[index] = acc.x();
      data[index + 1] = acc.y();
      data[index + 2] = acc.z();
    }
  }
}
}  // namespace procrock