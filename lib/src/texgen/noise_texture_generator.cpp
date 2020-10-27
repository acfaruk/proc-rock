#include "texgen/noise_texture_generator.h"

namespace procrock {
NoiseTextureGenerator::NoiseTextureGenerator() {
  noiseGraph.addOwnGroups(config, "Displacement / Height");

  albedoGenerator.addOwnGroups(config, "Albedo");
  normalsGenerator.addOwnGroups(config, "Normals");
  roughnessGenerator.addOwnGroups(config, "Roughness");
  metalnessGenerator.addOwnGroups(config, "Metalness");
  ambientOccGenerator.addOwnGroups(config, "Ambient Occlusion");
}

std::shared_ptr<Mesh> NoiseTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  auto noiseModule = evaluateGraph(noiseGraph);
  auto heightFunction = [&](Eigen::Vector3f worldPos) {
    if (noiseModule == nullptr) return 0.0f;
    float value = (noiseModule->GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    return std::max(0.0f, std::min(value, 1.0f));
  };

  fillTexture(result->textures, heightFunction);

  albedoGenerator.modify(result->textures);
  normalsGenerator.modify(result->textures);
  roughnessGenerator.modify(result->textures);
  metalnessGenerator.modify(result->textures);
  ambientOccGenerator.modify(result->textures);

  return result;
}
PipelineStageInfo& NoiseTextureGenerator::getInfo() { return info; }

}  // namespace procrock