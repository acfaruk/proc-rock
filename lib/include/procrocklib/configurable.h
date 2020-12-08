#pragma once

#include <Eigen/Core>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace procrock {
struct NoiseGraph;

template <typename T>
struct ConfigurationList {
  ConfigurationList(std::set<T> initial = {}, int min = 0, int max = 100)
      : list(initial), minEntries(min), maxEntries(max){};

  int minEntries;
  int maxEntries;

  void insert(T value) {
    if (list.size() < maxEntries) list.insert(value);
  }
  void erase(T value) {
    if (list.size() > minEntries) list.erase(value);
  }

  const std::set<T>& values() { return list; }
  friend void to_json(nlohmann::json& j, const ConfigurationList<T>& list);
  friend void from_json(const nlohmann::json& j, ConfigurationList<T>& list);

 private:
  std::set<T> list;
};

struct ConfigurationCurve {
  ConfigurationCurve(std::map<float, float> initial = {}) : curvePoints(initial){};

  void add(float pos, float value) {
    if (pos <= 1.0f && pos >= 0.0f && value <= 1.0f && value >= 0.0f) {
      curvePoints[pos] = value;
    }
  }

  void remove(float& pos) {
    if (curvePoints.size() > 4) {
      curvePoints.erase(pos);
    }
  }

  const std::map<float, float>& values() { return curvePoints; }
  friend void to_json(nlohmann::json& j, const ConfigurationCurve& curve);
  friend void from_json(const nlohmann::json& j, ConfigurationCurve& curve);

 private:
  std::map<float, float> curvePoints;
};

struct Configuration {
  struct Entry {
    std::string name;
    std::string description;
    std::function<bool()> active = []() { return true; };
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
  typedef SimpleEntry<std::map<int, Eigen::Vector4f>> GradientAlphaColoringEntry;
  typedef SimpleEntry<std::map<float, float>> CurvePositionsEntry;

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
    std::vector<GradientAlphaColoringEntry> gradientAlphaColorings;
    std::vector<SimpleEntry<ConfigurationCurve>> curves;

    inline int* getInt(std::string value) {
      for (int i = 0; i < ints.size(); i++) {
        if (ints[i].entry.active() && ints[i].entry.name == value) {
          return ints[i].data;
        }
      }
      return nullptr;
    }
    inline float* getFloat(std::string value) {
      for (int i = 0; i < floats.size(); i++) {
        if (floats[i].entry.active() && floats[i].entry.name == value) {
          return floats[i].data;
        }
      }
      return nullptr;
    }
    inline Eigen::Vector3f* getFloat3(std::string value) {
      for (int i = 0; i < float3s.size(); i++) {
        if (float3s[i].entry.active() && float3s[i].entry.name == value) {
          return float3s[i].data;
        }
      }
      return nullptr;
    }
    inline bool* getBool(std::string value) {
      for (int i = 0; i < bools.size(); i++) {
        if (bools[i].entry.active() && bools[i].entry.name == value) {
          return bools[i].data;
        }
      }
      return nullptr;
    }
    inline NoiseGraph* getNoiseGraph(std::string value) {
      for (int i = 0; i < noiseGraphs.size(); i++) {
        if (noiseGraphs[i].entry.active() && noiseGraphs[i].entry.name == value) {
          return noiseGraphs[i].data;
        }
      }
      return nullptr;
    }
    inline ConfigurationList<float>* getFloatList(std::string value) {
      for (int i = 0; i < floatLists.size(); i++) {
        if (floatLists[i].entry.active() && floatLists[i].entry.name == value) {
          return floatLists[i].data;
        }
      }
      return nullptr;
    }
    inline int* getSingleChoiceEntry(std::string value) {
      for (int i = 0; i < singleChoices.size(); i++) {
        if (singleChoices[i].entry.active() && singleChoices[i].entry.name == value) {
          return singleChoices[i].choice;
        }
      }
      return nullptr;
    }
    inline std::map<int, Eigen::Vector3f>* getGradientColoring(std::string value) {
      for (int i = 0; i < gradientColorings.size(); i++) {
        if (gradientColorings[i].entry.active() && gradientColorings[i].entry.name == value) {
          return gradientColorings[i].data;
        }
      }
      return nullptr;
    }
    inline std::map<int, Eigen::Vector4f>* getGradientAlphaColoring(std::string value) {
      for (int i = 0; i < gradientAlphaColorings.size(); i++) {
        if (gradientAlphaColorings[i].entry.active() &&
            gradientAlphaColorings[i].entry.name == value) {
          return gradientAlphaColorings[i].data;
        }
      }
      return nullptr;
    }
    inline ConfigurationCurve* getConfigurationCurve(std::string value) {
      for (int i = 0; i < curves.size(); i++) {
        if (curves[i].entry.active() && curves[i].entry.name == value) {
          return curves[i].data;
        }
      }
      return nullptr;
    }
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
  inline const std::vector<ConfigurationGroupsElement>& getConfigGroupsConst() const {
    return configGroups;
  }

  inline ConfigurationGroup getConfigGroup(std::string main, std::string sub) {
    for (int i = 0; i < configGroups.size(); i++) {
      if (configGroups[i].first == main) {
        auto& mainGroup = configGroups[i].second;
        for (int j = 0; j < mainGroup.size(); j++) {
          if (mainGroup[j].entry.active() && mainGroup[j].entry.name == sub) {
            return mainGroup[j];
          }
        }
      }
    }

    return ConfigurationGroup();
  }
};

class Configurable {
 public:
  inline void setChanged(bool changed) { this->changed = changed; }
  inline bool isChanged() { return changed; }
  inline const Configuration& getConfiguration() { return config; }

 protected:
  Configuration config;

 private:
  bool changed = false;
};

}  // namespace procrock