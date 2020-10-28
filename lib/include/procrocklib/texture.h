#pragma once

#include <Eigen/Core>
#include <array>
#include <vector>

namespace procrock {
struct TextureGroup {
  unsigned int width = 512;
  unsigned int height = 512;
  unsigned int albedoChannels = 3;

  struct WorldMapEntry {
    std::array<Eigen::Vector3f, 9> positions;
    int face = -1;
  };
  std::vector<WorldMapEntry> worldMap;

  std::vector<unsigned char> albedoData;
  std::vector<float> displacementData;
  std::vector<unsigned char> normalData;
  std::vector<unsigned char> roughnessData;
  std::vector<unsigned char> metalData;
  std::vector<unsigned char> ambientOccData;
};
}  // namespace procrock