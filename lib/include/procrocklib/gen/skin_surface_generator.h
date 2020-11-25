#pragma once
#include <CGAL/Random.h>
#include <procrocklib/generator.h>

namespace procrock {
class SkinSurfaceGenerator : public Generator {
 public:
  SkinSurfaceGenerator();

  int distributionMethod = 0;

  int pointAmount = 20;
  int seed = 0;

  float pointSize = 0.2;
  float shrinkFactor = 0.5;

  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Gen[PipelineStage_Gen_SkinSurface],
                         "Generates a skin surface.", PipelineStageType::Generator,
                         PipelineStage_Gen_SkinSurface};
};
}  // namespace procrock