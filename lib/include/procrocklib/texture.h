#pragma once

#include <vector>

namespace procrock {
struct Texture {
  unsigned int width = 1024;
  unsigned int height = 1024;

  std::vector<unsigned char> data;
};
}  // namespace procrock