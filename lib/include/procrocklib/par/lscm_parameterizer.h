#pragma once
#include <procrocklib/parameterizer.h>

#include <random>

namespace procrock {
class LSCM_Parameterizer : public Parameterizer {
 public:
  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  virtual std::shared_ptr<Mesh> parameterize(Mesh* mesh) override;

 private:
  PipelineStageInfo info{PipelineStage_Par_LSCM, "Least Squares Conformal Maps"};
};
}  // namespace procrock