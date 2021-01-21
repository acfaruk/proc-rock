#pragma once

#include <procrocklib/modifier.h>

namespace procrock {
class DecimateModifier : public Modifier {
 public:
  DecimateModifier();
  virtual ~DecimateModifier() = default;

  int mode = 0;  // 0 = relative value, 1 = absolute value

  float relativeValue = 0.5;
  int absoluteValue = 1000;

  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) override;
  virtual PipelineStageInfo& getInfo() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Mod[PipelineStage_Mod_Decimate],
                         "Decimate the mesh to lower the face count.", PipelineStageType::Modifier,
                         PipelineStage_Mod_Decimate};

  int faceCount = 0;
};
}  // namespace procrock