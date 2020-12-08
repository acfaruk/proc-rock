#pragma once
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/gen/skin_surface_generator.h>
#include <procrocklib/mod/all.h>
#include <procrocklib/texgen/noise_texture_generator.h>

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

  NoiseTextureGenerator* textureGenerator;

  int seed = 0;
  bool cutGround = true;

  int grainChoice = 0;  // 0 = fine grained, 1 = coarse, 2 = two distinct
};
}  // namespace procrock
