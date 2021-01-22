#pragma once

#include <Eigen/Core>
#include <algorithm>
#include <map>

namespace procrock {
namespace utils {
Eigen::Vector3i inline computeColorGradient(std::map<int, Eigen::Vector3f>& gradient, int min,
                                            int max, float value) {
  float cappedValue = std::min(1.0f, std::max(0.0f, value));
  int gradientValue = 100 * cappedValue;

  if (gradient.size() == 0) {
    return Eigen::Vector3i{0, 0, 0};
  }
  if (gradient.size() == 1) {
    return (255 * gradient.begin()->second).cast<int>();
  }

  auto upper = gradient.upper_bound(gradientValue);

  if (upper == gradient.end()) {
    upper--;
    return (255 * upper->second).cast<int>();
  }

  int secondValue = upper->first;
  Eigen::Vector3f secondColor = upper->second;

  if (upper != gradient.begin()) {
    upper--;
  }

  int firstValue = upper->first;
  Eigen::Vector3f firstColor = upper->second;

  float fraction = (gradientValue - firstValue) / (float)(secondValue - firstValue);

  return (255 * ((secondColor - firstColor) * fraction + firstColor)).cast<int>();
}

Eigen::Vector4i inline computeAlphaColorGradient(std::map<int, Eigen::Vector4f>& gradient, int min,
                                                 int max, float value) {
  float cappedValue = std::min(1.0f, std::max(0.0f, value));
  int gradientValue = 100 * cappedValue;

  if (gradient.size() == 0) {
    return Eigen::Vector4i{0, 0, 0, 0};
  }
  if (gradient.size() == 1) {
    return (255 * gradient.begin()->second).cast<int>();
  }

  auto upper = gradient.upper_bound(gradientValue);

  if (upper == gradient.end()) {
    upper--;
    return (255 * upper->second).cast<int>();
  }

  int secondValue = upper->first;
  Eigen::Vector4f secondColor = upper->second;

  if (upper != gradient.begin()) {
    upper--;
  }

  int firstValue = upper->first;
  Eigen::Vector4f firstColor = upper->second;

  float fraction = (gradientValue - firstValue) / (float)(secondValue - firstValue);

  return (255 * ((secondColor - firstColor) * fraction + firstColor)).cast<int>();
}
}  // namespace utils

}  // namespace procrock