#pragma once

#include <procrocklib/configurables/coloring.h>
#include <procrocklib/configurables/noise.h>
#include <procrocklib/texture_generator.h>

namespace procrock {

class SingleNoiseTextureGenerator : public TextureGenerator {
 public:
  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexGen[PipelineStage_TexGen_SingleNoise],
                         "Generates Solid Noise Textures based on a single noise function."};

  SingleNoiseModule module;
  GradientColoring coloring;
};
}  // namespace procrock