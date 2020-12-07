#pragma once
#include <procrocklib/gen/skin_surface_generator.h>
#include <procrocklib/mod/all.h>

#include "abstracted_pipeline.h"

namespace procrock {
class IgneousPipeline : public AbstractedPipeline {
 public:
  IgneousPipeline();

 private:
  virtual void setupPipeline() override;
  virtual void updatePipeline() override;

  SkinSurfaceGenerator* generator;

  CutPlaneModifier* modCutGround;
  DecimateModifier* modDecimate;
  TransformationModifier* modTransform;

  int seed = 0;
  bool cutGround = true;
};
}  // namespace procrock
