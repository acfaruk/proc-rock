#pragma once

#include <Eigen/Core>
#include <map>
#include <string>
#include <utility>
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

  struct GradientColoringEntry {
    Entry entry;
    std::map<int, Eigen::Vector3f>* colors;
  };

  template <typename T>
  struct BoundedEntry {
    Entry entry;
    T* data;
    T min, max;
  };

  template <typename T>
  struct SimpleEntry {
    Entry entry;
    T* data;
  };

  struct ConfigurationGroup {
    Entry entry;
    std::vector<BoundedEntry<int>> ints;
    std::vector<BoundedEntry<float>> floats;

    std::vector<SimpleEntry<bool>> bools;

    std::vector<SingleChoiceEntry> singleChoices;
    std::vector<GradientColoringEntry> gradientColorings;
  };

  std::map<std::string, std::vector<ConfigurationGroup>> configGroups;
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