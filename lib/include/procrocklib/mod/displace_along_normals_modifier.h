#pragma once
#include <procrocklib/modifier.h>

#include <random>

namespace procrock {
class DisplaceAlongNormalsModifier : public Modifier {
 public:
  int seed = 0;
  float factor = 0.001;

  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) override;
  Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Mod[PipelineStage_Mod_DisplaceAlongNormals],
                         "Displace all vertices allong their respective normals"};

  std::mt19937 rng;
};
}  // namespace procrock