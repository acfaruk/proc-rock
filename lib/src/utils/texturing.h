#pragma once
#include <procrocklib/mesh.h>

#include <thread>

#include "utils/vector.h"

namespace procrock {
namespace utils {
typedef std::function<float(Eigen::Vector3f)> FloatTextureFunction;

inline void fillPart(std::vector<float>& data, int startIndex, int endIndex,
                     const std::vector<TextureGroup::WorldMapEntry>& entries,
                     FloatTextureFunction texFunction) {
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

inline void fillFloatTexture(TextureGroup& texGroup, FloatTextureFunction texFunction,
                             std::vector<float>& dataToFill) {
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
    threads.emplace_back(std::thread(&fillPart, std::ref(data[i]), startIndex,
                                     startIndex + data[i].size(), std::cref(texGroup.worldMap),
                                     texFunction));
  }

  dataToFill.clear();
  for (int i = 0; i < data.size(); i++) {
    threads[i].join();
    dataToFill.insert(dataToFill.end(), data[i].begin(), data[i].end());
  }
}

}  // namespace utils
}  // namespace procrock