#pragma once
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/gen/skin_surface_generator.h>
#include <procrocklib/mod/all.h>
#include <procrocklib/texadd/all.h>
#include <procrocklib/texgen/noise_texture_generator.h>

#include "abstracted_pipeline.h"
#include "configurables/form_generator.h"
#include "configurables/texture_extras.h"

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

  SkinSurfaceGenerator* generator;

  SubdivisionModifier* modSubdivision;
  DisplaceAlongNormalsModifier* modDisplaceAlongNormals;

  NoiseTextureGenerator* textureGenerator;

  NoiseTextureAdder* textureAdderGrainsSecondary;
  NoiseTextureAdder* textureAdderGrainsTertiary;
  NoiseTextureAdder* textureAdderGrainsQuaternary;

  FormGeneratorExtender formGeneratorExtender;
  TextureExtrasExtender textureExtrasExtender;

  float baseGrainSize = 0.33;

  bool layered = true;
  float layerFrequency = 6.0;
  float layerStrength = 0.15;

  Eigen::Vector3f baseColor = {0.532, 0.355, 0.084};
  Eigen::Vector3f secondaryColor = {0.681, 0.681, 0.681};
  Eigen::Vector3f tertiaryColor = {0.0, 0.0, 0.0};

  bool useSecondaryGrains = true;
  Eigen::Vector3f secondaryGrainsColor = {0.5, 0.3, 0.1};
  float secondaryGrainSize = 0.6;

  bool useTertiaryGrains = false;
  Eigen::Vector3f tertiaryGrainsColor = {0.8, 0.8, 0.8};
  float tertiaryGrainSize = 0.5;

  bool useQuaternaryGrains = false;
  Eigen::Vector3f quaternaryGrainsColor = {0.5, 0.3, 0.1};
  float quaternaryGrainSize = 0.5;
};
}  // namespace procrock
