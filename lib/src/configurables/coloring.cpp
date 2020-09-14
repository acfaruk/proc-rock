#include "configurables/coloring.h"

#include "utils/colors.h"

namespace procrock {
void GradientColoring::addOwnGroups(Configuration& config, std::string newGroupName) {
  Configuration::ConfigurationGroup colorGroup;
  colorGroup.entry = {"Coloring Settings", "Create colors for the texture."};
  colorGroup.gradientColorings.push_back(Configuration::GradientColoringEntry(
      {{"Gradient",
        "Color the texture according to a user defined gradient. Color values are defined over the "
        "range 0-100."},
       &colorGradient}));

  config.insertToConfigGroups(newGroupName + ": Gradient Coloring", colorGroup);
}
Eigen::Vector3i GradientColoring::colorFromValue(float value) {
  return utils::computeColorGradient(colorGradient, 0, 100, value);
}
}  // namespace procrock