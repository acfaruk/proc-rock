#pragma once

#include <procrocklib/configurables/coloring.h>
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/texture_generator.h>

namespace procrock {

class IgneousTextureGenerator : public TextureGenerator {
 public:
  IgneousTextureGenerator();

  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexGen[PipelineStage_TexGen_Igneous],
                         "Generates textures based on igneous rock properties.",
                         PipelineStageType::TextureGenerator, PipelineStage_TexGen_Igneous};

  float grainSize = 0.7;
  float mineralComposition = 0.2;

  NoiseGraph noiseGraph;
  GradientColoring coloring;

  VoronoiNoiseNode* voronoiModule;
  PerlinNoiseNode* perlinModule;
  TurbulenceNoiseNode* turbulenceModule;

  GradientAlbedoGenerator albedoGenerator;
  GradientNormalsGenerator normalsGenerator;
  GreyscaleRoughnessGenerator roughnessGenerator;
  MetalnessGenerator metalnessGenerator;
  AmbientOcclusionGenerator ambientOccGenerator;
};
}  // namespace procrock