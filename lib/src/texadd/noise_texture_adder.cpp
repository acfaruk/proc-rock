#include "texadd/noise_texture_adder.h"

namespace procrock {
std::shared_ptr<Mesh> NoiseTextureAdder::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  auto noiseModule = evaluateGraph(noiseGraph);
  auto colorFunction = [&](Eigen::Vector3d worldPos) {
    if (noiseModule == nullptr) return coloring.colorFromValue(0);
    float value = (noiseModule->GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    return coloring.colorFromValue(value);
  };

  addTexture(*result, colorFunction);
  return result;
}

PipelineStageInfo& NoiseTextureAdder::getInfo() { return info; }

Configuration NoiseTextureAdder::getConfiguration() {
  Configuration result = getBaseConfiguration();
  std::string baseGroupName = "Noise Function";
  noiseGraph.addOwnGroups(result, "Noise");
  coloring.addOwnGroups(result, "Coloring Settings");
  return result;
}
}  // namespace procrock