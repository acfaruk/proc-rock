#pragma once

#include <string>
#include <vector>

namespace procrock {
struct Configuration {
  template <typename T>
  struct Entry {
    std::string name;
    std::string description;
    T* data;
  };
  std::vector<Entry<int>> ints;
  std::vector<Entry<float>> floats;
};
class Configurable {
 public:
  virtual Configuration getConfiguration() = 0;
};
}  // namespace procrock