#pragma once

#include <procrocklib/configurables/coloring.h>
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/texture_adder.h>

namespace procrock {

class NoiseTextureAdder : public TextureAdder {
 public:
  NoiseTextureAdder();
  virtual ~NoiseTextureAdder() = default;

  virtual PipelineStageInfo& getInfo() override;
  NoiseGraph noiseGraph;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  AlbedoAlphaGenerator albedoGenerator;
  NormalsGenerator normalsGenerator;
  RoughnessGenerator roughnessGenerator;
  MetalnessGenerator metalnessGenerator;
  AmbientOcclusionGenerator ambientOccGenerator;

  PipelineStageInfo info{PipelineStageNames_TexAdd[PipelineStage_TexAdd_Noise],
                         "Adds Solid Noise Texture based on a noise graph.",
                         PipelineStageType::TextureAdder, PipelineStage_TexAdd_Noise};
};
}  // namespace procrock