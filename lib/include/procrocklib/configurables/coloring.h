#pragma once

#include "configurable_extender.h"

namespace procrock {
class ValueColoring : public ConfigurableExtender {
 public:
  virtual Eigen::Vector3i colorFromValue(float value) = 0;
};

class ValueAlphaColoring : public ConfigurableExtender {
 public:
  virtual Eigen::Vector4i colorFromValue(float value) = 0;
};

class GradientColoring : public ValueColoring {
 public:
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual Eigen::Vector3i colorFromValue(float value) override;

  std::map<int, Eigen::Vector3f> colorGradient{{0, {0.827, 0.784, 0.517}},
                                               {30, {0.286, 0.225, 0.225}},
                                               {45, {0.427, 0.395, 0.395}},
                                               {65, {0.569, 0.553, 0.378}},
                                               {100, {0.940, 0.936, 0.921}}};
};

class GradientAlphaColoring : public ValueAlphaColoring {
 public:
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual Eigen::Vector4i colorFromValue(float value) override;

  std::map<int, Eigen::Vector4f> colorGradient{{0, {0.827, 0.784, 0.517, 0.1}},
                                               {30, {0.286, 0.225, 0.225, 0.3}},
                                               {45, {0.427, 0.395, 0.395, 0.45}},
                                               {65, {0.569, 0.553, 0.378, 0.65}},
                                               {100, {0.940, 0.936, 0.921, 1}}};
};

}  // namespace procrock