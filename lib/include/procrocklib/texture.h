#pragma once

#include <Eigen/Core>
#include <vector>

namespace procrock {
struct TextureGroup {
  unsigned int width = 256;
  unsigned int height = 256;
  unsigned int albedoChannels = 3;

  struct WorldMapEntry {
    std::vector<Eigen::Vector3d> positions;
    int face;
  };
  std::vector<WorldMapEntry> worldMap;

  std::vector<unsigned char> albedoData;
  std::vector<unsigned char> normalData;
  std::vector<unsigned char> roughnessData;
  std::vector<unsigned char> metalData;
  std::vector<unsigned char> ambientOccData;
};
}  // namespace procrock