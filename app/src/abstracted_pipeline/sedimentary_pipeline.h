#pragma once
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/gen/skin_surface_generator.h>
#include <procrocklib/mod/all.h>
#include <procrocklib/texadd/all.h>
#include <procrocklib/texgen/noise_texture_generator.h>

#include "abstracted_pipeline.h"

namespace procrock {
class SedimentaryPipeline : public AbstractedPipeline {
 public:
  SedimentaryPipeline();

 private:
  virtual void setupPipeline() override;
  virtual void updatePipeline() override;

  void updateTextureGenerator();
  void updateTextureAdderVariance();
  void updateTextureAdderMoss();

  SkinSurfaceGenerator* generator;

  CutPlaneModifier* modCutGround;
  DecimateModifier* modDecimate;
  TransformationModifier* modTransform;

  NoiseTextureGenerator* textureGenerator;

  NoiseTextureAdder* textureAdderVariance;
  NoiseTextureAdder* textureAdderMoss;

  int seed = 0;
  bool cutGround = true;


  bool textureVariance = true;
  bool textureMoss = false;

  Eigen::Vector3f baseColor = {0.4, 0.2, 0.1};
  Eigen::Vector3f secondaryColor = {1.0, 1.0, 1.0};
  Eigen::Vector3f tertiaryColor = {0.0, 0.0, 0.0};
};
}  // namespace procrock
