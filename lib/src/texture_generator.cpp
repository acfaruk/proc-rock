#include "texture_generator.h"

#include <CImg.h>
#include <igl/barycentric_coordinates.h>

#include <iostream>
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

void TextureGenerator::fillTexture(TextureGroup& texGroup, TextureFunction texFunction) {
  int index = 0;
  auto& dataToFill = texGroup.displacementData;
  dataToFill.resize(texGroup.width * texGroup.height);
  std::fill(dataToFill.begin(), dataToFill.end(), 0);

  const auto threadCount = std::thread::hardware_concurrency();
  int batchCount = dataToFill.size() / threadCount;

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
                                     texFunction));
  }

  dataToFill.clear();
  for (int i = 0; i < data.size(); i++) {
    threads[i].join();
    dataToFill.insert(dataToFill.end(), data[i].begin(), data[i].end());
  }
}

void TextureGenerator::fillPart(std::vector<float>& data, int startIndex, int endIndex,
                                const std::vector<TextureGroup::WorldMapEntry>& entries,
                                TextureFunction texFunction) {
  for (int i = startIndex; i < endIndex; i++) {
    const auto& pixel = entries[i];

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