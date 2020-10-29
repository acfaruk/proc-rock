#pragma once

#include <procrocklib/modifier.h>

namespace procrock {
class CutPlaneModifier : public Modifier {
 public:
  CutPlaneModifier();

  Eigen::Vector3f translation{0, 0, 0};
  Eigen::Vector3f rotation{0.3, 0, 1.7};

  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) override;
  virtual PipelineStageInfo& getInfo() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Mod[PipelineStage_Mod_CutPlane],
                         "Cut the mesh along a Plane", PipelineStageType::Modifier,
                         PipelineStage_Mod_CutPlane};
};
}  // namespace procrock