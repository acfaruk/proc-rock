#include "texgen/single_noise_texture_generator.h"

namespace procrock {
std::shared_ptr<Mesh> SingleNoiseTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  auto noiseModule = module.getModule();
  auto colorFunction = [&](Eigen::Vector3d worldPos) {
    float value = (noiseModule->GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    return coloring.colorFromValue(value);
  };

  fillTexture(result->textures, result->textures.albedoData, colorFunction);
  return result;
}

PipelineStageInfo& SingleNoiseTextureGenerator::getInfo() { return info; }

Configuration SingleNoiseTextureGenerator::getConfiguration() {
  Configuration result = getBaseConfiguration();
  std::string baseGroupName = "Noise Function";
  module.addOwnGroups(result, baseGroupName);
  coloring.addOwnGroups(result, "Coloring Settings");
  return result;
}
}  // namespace procrock