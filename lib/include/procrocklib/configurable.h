#pragma once

#include <string>
#include <vector>

namespace procrock {
struct Configuration {
  template <typename T>
  struct BoundedEntry {
    std::string name;
    std::string description;
    T* data;
    T min, max;
  };
  std::vector<BoundedEntry<int>> ints;
  std::vector<BoundedEntry<float>> floats;
};
class Configurable {
 public:
  virtual Configuration getConfiguration() = 0;

  void inline setChanged(bool changed) { this->changed = changed; }
  bool inline isChanged() { return changed; }

 private:
  bool changed = false;
};
}  // namespace procrock