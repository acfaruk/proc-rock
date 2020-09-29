#pragma once

#include <procrocklib/configurables/coloring.h>
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/texture_generator.h>

namespace procrock {

class NoiseTextureGenerator : public TextureGenerator {
 public:
  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexGen[PipelineStage_TexGen_Noise],
                         "Generates Solid Noise Textures based on a noise graph.",
                         PipelineStageType::TextureGenerator, PipelineStage_TexGen_Noise};

  NoiseGraph noiseGraph;
  GradientColoring coloring;
};
}  // namespace procrock