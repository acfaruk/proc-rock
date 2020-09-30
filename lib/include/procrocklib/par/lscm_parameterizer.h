#pragma once
#include <procrocklib/parameterizer.h>

#include <random>

namespace procrock {
class LSCM_Parameterizer : public Parameterizer {
 public:
  LSCM_Parameterizer();

  float scaling = 1;

  virtual PipelineStageInfo& getInfo() override;

 protected:
  virtual std::shared_ptr<Mesh> parameterize(Mesh* mesh) override;

 private:
  int indexMax = 0;
  PipelineStageInfo info{PipelineStageNames_Par[PipelineStage_Par_LSCM],
                         "Least Squares Conformal Maps", PipelineStageType::Parameterizer,
                         PipelineStage_Par_LSCM};
};
}  // namespace procrock