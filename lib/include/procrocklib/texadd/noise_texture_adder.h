#pragma once

#include <procrocklib/configurables/coloring.h>
#include <procrocklib/configurables/noise.h>
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/texture_adder.h>

namespace procrock {

class NoiseTextureAdder : public TextureAdder {
 public:
  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexAdd[PipelineStage_TexAdd_Noise],
                         "Adds Solid Noise Texture based on a noise graph."};

  NoiseGraph noiseGraph;
  GradientAlphaColoring coloring;
};
}  // namespace procrock