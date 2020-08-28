#pragma once
#include <procrocklib/parameterizer.h>

namespace procrock {
class XAtlasParameterizer : public Parameterizer {
 public:
  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  virtual std::shared_ptr<Mesh> parameterize(Mesh* mesh) override;

 private:
  PipelineStageInfo info{PipelineStageNames_Par[PipelineStage_Par_XATLAS], "xatlas Library"};
};
}  // namespace procrock