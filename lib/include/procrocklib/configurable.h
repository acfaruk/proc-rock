#pragma once

#include <string>
#include <vector>

namespace procrock {
struct Configuration {
  struct Entry {
    std::string name;
    std::string description;
  };

  struct SingleChoiceEntry {
    Entry entry;
    std::vector<Entry> choices;
    int* choice;
  };

  template <typename T>
  struct BoundedEntry {
    Entry entry;
    T* data;
    T min, max;
  };
  std::vector<BoundedEntry<int>> ints;
  std::vector<BoundedEntry<float>> floats;

  std::vector<SingleChoiceEntry> singleChoices;
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