#include "texadd/noise_texture_adder.h"

namespace procrock {
NoiseTextureAdder::NoiseTextureAdder() {
  noiseGraph.addOwnGroups(config, "Displacement / Height");

  albedoGenerator.addOwnGroups(config, "Albedo");
  normalsGenerator.addOwnGroups(config, "Normals");
  roughnessGenerator.addOwnGroups(config, "Roughness");
  metalnessGenerator.addOwnGroups(config, "Metalness");
  ambientOccGenerator.addOwnGroups(config, "Ambient Occlusion");
}

std::shared_ptr<Mesh> NoiseTextureAdder::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  auto noiseModule = evaluateGraph(noiseGraph);
  auto colorFunction = [&](Eigen::Vector3f worldPos) {
    if (noiseModule == nullptr) return 0.0f;
    float value = (noiseModule->GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    return std::max(0.0f, std::min(value, 1.0f));
  };

  auto texGroup = createAddTexture(*result, colorFunction);
  albedoGenerator.modify(texGroup);
  normalsGenerator.modify(texGroup);
  roughnessGenerator.modify(texGroup);
  metalnessGenerator.modify(texGroup);
  ambientOccGenerator.modify(texGroup);
  addTextures(*result, texGroup);
  return result;
}

PipelineStageInfo& NoiseTextureAdder::getInfo() { return info; }

}  // namespace procrock