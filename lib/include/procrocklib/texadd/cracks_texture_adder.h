#pragma once

#include <procrocklib/configurables/coloring.h>
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/texture_adder.h>

namespace procrock {

class CracksTextureAdder : public TextureAdder {
 public:
  CracksTextureAdder();
  virtual ~CracksTextureAdder() = default;

  virtual PipelineStageInfo& getInfo() override;

  float frequency = 3;
  float strength = 20;
  int seed = 0;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexAdd[PipelineStage_TexAdd_Cracks],
                         "Adds Cracks to the texture.", PipelineStageType::TextureAdder,
                         PipelineStage_TexAdd_Cracks};
};
}  // namespace procrock