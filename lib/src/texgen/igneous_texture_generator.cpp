#include "texgen/igneous_texture_generator.h"

namespace procrock {
IgneousTextureGenerator::IgneousTextureGenerator() {
  noiseGraph.clear();

  int outputNode = noiseGraph.addNode(createNoiseNodeFromTypeId(NoiseNodeTypeId_Output), true);

  auto voronoi = std::make_unique<VoronoiNoiseNode>();
  voronoiModule = voronoi.get();
  int voronoiNode = noiseGraph.addNode(std::move(voronoi));

  auto perlin = std::make_unique<PerlinNoiseNode>();
  perlinModule = perlin.get();
  int perlinNode = noiseGraph.addNode(std::move(perlin));

  int addNode = noiseGraph.addNode(createNoiseNodeFromTypeId(NoiseNodeTypeId_Add));

  auto turbulence = std::make_unique<TurbulenceNoiseNode>();
  turbulenceModule = turbulence.get();
  int turbulenceNode = noiseGraph.addNode(std::move(turbulence));

  noiseGraph.addEdge(turbulenceNode, outputNode);
  noiseGraph.addEdge(addNode, turbulenceNode);
  noiseGraph.addEdge(voronoiNode, addNode, 0);
  noiseGraph.addEdge(perlinNode, addNode, 1);

  Configuration::ConfigurationGroup mainGroup;
  mainGroup.entry = {"General Settings", "General Settings for the igneous rock."};
  mainGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Grain Size",
       "Choose how big the grains in the igneous rock are. A value of 1 is a very coarse-grained "
       "Phaneritic rock, where as a value of 0 is a very fine grained Aphanitic rock."},
      &grainSize,
      0,
      1});

  mainGroup.floats.emplace_back(
      Configuration::BoundedEntry<float>{{"Composition",
                                          "A value of 0 represents a very Felsic / Granitic rock, "
                                          "where as a value of 1 is a very Mafic / Basaltic one."},
                                         &mineralComposition,
                                         0.001,
                                         1});

  config.insertToConfigGroups("General", mainGroup);
  // noiseGraph.addOwnGroups(config, "Noise");
  coloring.addOwnGroups(config, "Coloring Settings");
}

std::shared_ptr<Mesh> IgneousTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  voronoiModule->frequency = 25 + ((1 - grainSize) * (1 - grainSize)) * (150 - 25);
  perlinModule->frequency = 100000;

  turbulenceModule->frequency = grainSize * (0.5);
  turbulenceModule->power = 0.1 * grainSize;
  turbulenceModule->roughness = std::max((int)(2 * grainSize), 1);

  normalsGenerator.normalStrength = 1.4 + (grainSize) * (1.45 - 1.4);
  roughnessGenerator.scaling = 10.0f;

  auto noiseModule = evaluateGraph(noiseGraph);
  auto colorFunction = [&](Eigen::Vector3f worldPos) {
    if (noiseModule == nullptr) return 0.0f;
    float value = (noiseModule->GetValue(worldPos.x(), worldPos.y(), worldPos.z()) + 1) / 2;
    return value / std::max((mineralComposition * 7), 1.0f);
  };

  fillTexture(result->textures, colorFunction);

  albedoGenerator.modify(result->textures);
  normalsGenerator.modify(result->textures);
  roughnessGenerator.modify(result->textures);
  metalnessGenerator.modify(result->textures);
  ambientOccGenerator.modify(result->textures);

  return result;
}
PipelineStageInfo& IgneousTextureGenerator::getInfo() { return info; }

}  // namespace procrock