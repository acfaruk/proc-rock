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

  // returns the id of the output node
  int createLayerNoise(NoiseGraph* noise);

  void updateModifierDisplaceAlongNormals();
  void updateTextureGenerator();
  void updateTextureAdderVariance();
  void updateTextureAdderMoss();

  SkinSurfaceGenerator* generator;

  CutPlaneModifier* modCutGround;
  SubdivisionModifier* modSubdivision;
  DisplaceAlongNormalsModifier* modDisplaceAlongNormals;
  DecimateModifier* modDecimate;
  TransformationModifier* modTransform;

  NoiseTextureGenerator* textureGenerator;

  NoiseTextureAdder* textureAdderVariance;
  NoiseTextureAdder* textureAdderMoss;

  int seed = 0;
  bool cutGround = true;

  bool layered = true;

  bool textureVariance = true;
  bool textureMoss = false;

  Eigen::Vector3f baseColor = {0.532, 0.355, 0.084};
  Eigen::Vector3f secondaryColor = {0.681, 0.681, 0.681};
  Eigen::Vector3f tertiaryColor = {0.0, 0.0, 0.0};
};
}  // namespace procrock
