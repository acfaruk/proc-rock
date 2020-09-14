#pragma once

#include <procrocklib/configurables/coloring.h>
#include <procrocklib/configurables/noise.h>
#include <procrocklib/texture_generator.h>

namespace procrock {
class SelectedNoiseTextureGenerator : public TextureGenerator {
 public:
  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexGen[PipelineStage_TexGen_SelectedNoise],
                         "Generates Solid Noise Textures by selecting a value from two noise "
                         "functions by using a third one."};

  SelectedNoiseModule module;
  GradientColoring coloring;
};
}  // namespace procrock