#pragma once

#include <vector>

namespace procrock {
struct Texture {
  unsigned int width = 256;
  unsigned int height = 256;

  std::vector<unsigned char> data;
};
}  // namespace procrock