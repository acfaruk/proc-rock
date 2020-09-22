#pragma once

#include <Eigen/Core>
#include <vector>

namespace procrock {
struct TextureGroup {
  unsigned int width = 256;
  unsigned int height = 256;
  unsigned int albedoChannels = 3;

  // Maps each pixel on the texture to a list of pairs which represent the world coordinate and the
  // world coordinates that make up that pixel
  std::vector<std::vector<Eigen::Vector3d>> worldMap;

  std::vector<unsigned char> albedoData;
  std::vector<unsigned char> normalData;
  std::vector<unsigned char> roughnessData;
  std::vector<unsigned char> metalData;
  std::vector<unsigned char> ambientOccData;
};
}  // namespace procrock