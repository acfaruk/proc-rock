#pragma once

#include <procrocklib/modifier.h>

namespace procrock {
class TransformationModifier : public Modifier {
 public:
  TransformationModifier();
  virtual ~TransformationModifier() = default;

  Eigen::Vector3f translation{0, 0, 0};
  Eigen::Vector3f scale{1, 1, 1};
  Eigen::Vector3f rotation{0, 0, 0};

  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) override;
  virtual PipelineStageInfo& getInfo() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Mod[PipelineStage_Mod_Transform],
                         "Translate, Rotate or Scale the rock.", PipelineStageType::Modifier,
                         PipelineStage_Mod_Transform};
};
}  // namespace procrock