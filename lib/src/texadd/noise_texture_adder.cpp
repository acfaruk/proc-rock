#include "texadd/noise_texture_adder.h"

namespace procrock {
NoiseTextureAdder::NoiseTextureAdder() {
  noiseGraph.addOwnGroups(config, "Noise");
  coloring.addOwnGroups(config, "Coloring Settings");

  normalsGenerator.addOwnGroups(config, "Normals");
  roughnessGenerator.addOwnGroups(config, "Roughness");
  metalnessGenerator.addOwnGroups(config, "Metalness");
  ambientOccGenerator.addOwnGroups(config, "Ambient Occlusion");
}

std::shared_ptr<Mesh> NoiseTextureAdder::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  auto noiseModule = evaluateGraph(noiseGraph);
  auto colorFunction = [&](Eigen::Vector3f worldPos) {
    if (noiseModule == nullptr) return coloring.colorFromValue(0);
    float value = (noiseModule->GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    return coloring.colorFromValue(value);
  };

  auto texGroup = createAddTexture(*result, colorFunction);
  normalsGenerator.modify(texGroup);
  roughnessGenerator.modify(texGroup);
  metalnessGenerator.modify(texGroup);
  ambientOccGenerator.modify(texGroup);
  addTextures(*result, texGroup);
  return result;
}

PipelineStageInfo& NoiseTextureAdder::getInfo() { return info; }

}  // namespace procrock