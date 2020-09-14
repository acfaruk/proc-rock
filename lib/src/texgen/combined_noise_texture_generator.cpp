#include "texgen/combined_noise_texture_generator.h"

namespace procrock {

std::shared_ptr<Mesh> CombinedNoiseTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  auto noiseModule = module.getModule();
  auto colorFunction = [&](Eigen::Vector3d worldPos) {
    float value = (noiseModule->GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    return coloring.colorFromValue(value);
  };

  fillTexture(result->textures, result->textures.albedoData, colorFunction);
  return result;
}

PipelineStageInfo& CombinedNoiseTextureGenerator::getInfo() { return info; }
Configuration CombinedNoiseTextureGenerator::getConfiguration() {
  Configuration result = getBaseConfiguration();
  module.addOwnGroups(result, "Combined Noise");
  coloring.addOwnGroups(result, "Coloring Settings");
  return result;
}
}  // namespace procrock