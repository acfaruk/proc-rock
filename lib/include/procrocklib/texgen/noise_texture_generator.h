#pragma once

#include <procrocklib/configurables/coloring.h>
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/texture_generator.h>

namespace procrock {

class NoiseTextureGenerator : public TextureGenerator {
 public:
  NoiseTextureGenerator();
  virtual ~NoiseTextureGenerator() = default;

  virtual PipelineStageInfo& getInfo() override;
  NoiseGraph noiseGraph;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexGen[PipelineStage_TexGen_Noise],
                         "Generates Solid Noise Textures based on a noise graph.",
                         PipelineStageType::TextureGenerator, PipelineStage_TexGen_Noise};

  AlbedoGenerator albedoGenerator;
  NormalsGenerator normalsGenerator;
  RoughnessGenerator roughnessGenerator;
  MetalnessGenerator metalnessGenerator;
  AmbientOcclusionGenerator ambientOccGenerator;
};
}  // namespace procrock