#pragma once

#include <Eigen/Core>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace procrock {
struct NoiseGraph;

template <typename T>
struct ConfigurationList {
  ConfigurationList(std::set<T> initial, int min = 0, int max = 100)
      : list(initial), minEntries(min), maxEntries(max){};

  int minEntries;
  int maxEntries;

  void insert(T value) {
    if (list.size() < maxEntries) list.insert(value);
  }
  void erase(T value) {
    if (list.size() > minEntries) list.erase(value);
  }

  const std::set<T>& const values() { return list; }

 private:
  std::set<T> list;
};

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
    T minValue, maxValue;
  };

  template <typename T>
  struct SimpleEntry {
    Entry entry;
    T* data;
  };

  template <typename T>
  struct ListEntry {
    Entry entry;
    ConfigurationList<T>* data;
    T minValue;
    T maxValue;
  };

  typedef SimpleEntry<std::map<int, Eigen::Vector3f>> GradientColoringEntry;

  struct ConfigurationGroup {
    Entry entry;
    std::vector<BoundedEntry<int>> ints;
    std::vector<BoundedEntry<float>> floats;

    std::vector<BoundedEntry<Eigen::Vector3f>> float3s;

    std::vector<SimpleEntry<bool>> bools;
    std::vector<SimpleEntry<NoiseGraph>> noiseGraphs;
    std::vector<ListEntry<float>> floatLists;

    std::vector<SingleChoiceEntry> singleChoices;
    std::vector<GradientColoringEntry> gradientColorings;
  };

 private:
  typedef std::pair<std::string, std::vector<ConfigurationGroup>> ConfigurationGroupsElement;
  std::vector<ConfigurationGroupsElement> configGroups;

 public:
  inline void insertToConfigGroups(std::string name, ConfigurationGroup& group) {
    int index = -1;
    for (int i = 0; i < configGroups.size(); i++) {
      if (configGroups[i].first == name) {
        index = i;
        break;
      }
    }

    if (index == -1) {
      configGroups.emplace_back(ConfigurationGroupsElement{name, {group}});
    } else {
      configGroups[index].second.push_back(group);
    }
  }
  inline std::vector<ConfigurationGroupsElement>& getConfigGroups() { return configGroups; }
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