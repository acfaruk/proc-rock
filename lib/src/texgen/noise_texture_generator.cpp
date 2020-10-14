#include "texgen/noise_texture_generator.h"

namespace procrock {
NoiseTextureGenerator::NoiseTextureGenerator() {
  noiseGraph.addOwnGroups(config, "Noise");
  coloring.addOwnGroups(config, "Coloring Settings");

  normalsGenerator.addOwnGroups(config, "Normals");
  roughnessGenerator.addOwnGroups(config, "Roughness");
  metalnessGenerator.addOwnGroups(config, "Metalness");
  ambientOccGenerator.addOwnGroups(config, "Ambient Occlusion");
}

std::shared_ptr<Mesh> NoiseTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  auto noiseModule = evaluateGraph(noiseGraph);
  auto colorFunction = [&](Eigen::Vector3f worldPos) {
    if (noiseModule == nullptr) return coloring.colorFromValue(0);
    float value = (noiseModule->GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    return coloring.colorFromValue(value);
  };

  fillTexture(result->textures, colorFunction);

  normalsGenerator.modify(result->textures);
  roughnessGenerator.modify(result->textures);
  metalnessGenerator.modify(result->textures);
  ambientOccGenerator.modify(result->textures);

  return result;
}
PipelineStageInfo& NoiseTextureGenerator::getInfo() { return info; }

}  // namespace procrock