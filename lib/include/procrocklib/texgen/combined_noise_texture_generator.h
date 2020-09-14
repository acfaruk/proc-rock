#pragma once

#include <procrocklib/configurables/coloring.h>
#include <procrocklib/configurables/noise.h>
#include <procrocklib/texture_generator.h>

namespace procrock {
class CombinedNoiseTextureGenerator : public TextureGenerator {
 public:
  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexGen[PipelineStage_TexGen_CombinedNoise],
                         "Generates Solid Noise Textures based on two noise functions."};

  CombinedNoiseModule module;
  GradientColoring coloring;
};
}  // namespace procrock