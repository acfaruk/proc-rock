#include "configurables/coloring.h"

#include "utils/colors.h"

namespace procrock {
void GradientColoring::addOwnGroups(Configuration& config, std::string newGroupName,
                                    std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup colorGroup;
  colorGroup.entry = {"Gradient Coloring", "Create colors for the texture.", activeFunc};
  colorGroup.gradientColorings.push_back(Configuration::GradientColoringEntry(
      {{"Gradient",
        "Color the texture according to a user defined gradient. Color values are defined over the "
        "range 0-100."},
       &colorGradient}));

  config.insertToConfigGroups(newGroupName, colorGroup);
}
Eigen::Vector3i GradientColoring::colorFromValue(float value) {
  return utils::computeColorGradient(colorGradient, 0, 100, value);
}
void GradientAlphaColoring::addOwnGroups(Configuration& config, std::string newGroupName,
                                         std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup colorGroup;
  colorGroup.entry = {"Gradient Alpha Coloring", "Create colors for the texture.", activeFunc};
  colorGroup.gradientAlphaColorings.push_back(Configuration::GradientAlphaColoringEntry(
      {{"Gradient",
        "Color the texture according to a user defined gradient. Color values are defined over the "
        "range 0-100."},
       &colorGradient}));

  config.insertToConfigGroups(newGroupName, colorGroup);
}
Eigen::Vector4i GradientAlphaColoring::colorFromValue(float value) {
  return utils::computeAlphaColorGradient(colorGradient, 0, 100, value);
}
}  // namespace procrock