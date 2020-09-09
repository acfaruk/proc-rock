#include "texgen/single_noise_texture_generator.h"

#include <noise/noise.h>

#include "utils/colors.h"

namespace procrock {
std::shared_ptr<Mesh> SingleNoiseTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  auto noiseModule = module.getModule();
  auto colorFunction = [&](Eigen::Vector3d worldPos) {
    float value = (noiseModule->GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    return utils::computeColorGradient(colorGradient, 0, 100, value);
  };

  fillTexture(result->textures, result->textures.albedoData, colorFunction);
  return result;
}

PipelineStageInfo& SingleNoiseTextureGenerator::getInfo() { return info; }

Configuration SingleNoiseTextureGenerator::getConfiguration() {
  Configuration::ConfigurationGroup colorGroup;
  colorGroup.entry = {"Coloring Settings", "Create colors for the texture."};
  colorGroup.gradientColorings.push_back(Configuration::GradientColoringEntry(
      {{"Gradient",
        "Color the texture according to a user defined gradient. Color values are defined over the "
        "range 0-100."},
       &colorGradient}));

  Configuration result = getBaseConfiguration();
  std::string baseGroupName = "Noise Function";
  module.addOwnGroups(result, baseGroupName);
  result.insertToConfigGroups("Coloring", colorGroup);
  return result;
}
}  // namespace procrock