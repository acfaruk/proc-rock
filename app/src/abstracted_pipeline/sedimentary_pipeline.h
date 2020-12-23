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

  void updateTextureAdderGrainsSecondary();
  void updateTextureAdderGrainsTertiary();
  void updateTextureAdderGrainsQuaternary();
  void updateTextureAdderVariance();
  void updateTextureAdderMoss();

  SkinSurfaceGenerator* generator;

  CutPlaneModifier* modCutGround;
  SubdivisionModifier* modSubdivision;
  DisplaceAlongNormalsModifier* modDisplaceAlongNormals;
  DecimateModifier* modDecimate;
  TransformationModifier* modTransform;

  NoiseTextureGenerator* textureGenerator;

  NoiseTextureAdder* textureAdderGrainsSecondary;
  NoiseTextureAdder* textureAdderGrainsTertiary;
  NoiseTextureAdder* textureAdderGrainsQuaternary;

  NoiseTextureAdder* textureAdderVariance;
  NoiseTextureAdder* textureAdderMoss;

  int seed = 0;
  bool cutGround = true;

  float baseGrainSize = 0.33;

  bool layered = true;
  float layerFrequency = 6.0;
  float layerStrength = 0.15;

  bool textureVariance = true;
  bool textureMoss = false;

  Eigen::Vector3f baseColor = {0.532, 0.355, 0.084};
  Eigen::Vector3f secondaryColor = {0.681, 0.681, 0.681};
  Eigen::Vector3f tertiaryColor = {0.0, 0.0, 0.0};

  bool useSecondaryGrains = true;
  Eigen::Vector3f secondaryGrainsColor = {0.5, 0.3, 0.1};
  float secondaryGrainSize = 0.6;

  bool useTertiaryGrains = false;
  Eigen::Vector3f tertiaryGrainsColor = {0.6, 0.2, 0.0};
  float tertiaryGrainSize = 0.9;

  bool useQuaternaryGrains = false;
  Eigen::Vector3f quaternaryGrainsColor = {0.7, 0.1, 0.3};
  float quaternaryGrainSize = 0.5;
};
}  // namespace procrock
