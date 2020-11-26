#pragma once
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/generator.h>

namespace procrock {
class NoiseSurfaceGenerator : public Generator {
 public:
  NoiseSurfaceGenerator();

  int resolution = 10;
  bool fillHoles = true;

  NoiseGraph noiseGraph;

  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Gen[PipelineStage_Gen_NoiseSurface],
                         "Generates a surface based on a noise function.",
                         PipelineStageType::Generator, PipelineStage_Gen_NoiseSurface};
};
}  // namespace procrock