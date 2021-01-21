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
class MetamorphicPipeline : public AbstractedPipeline {
 public:
  MetamorphicPipeline();
  virtual ~MetamorphicPipeline() = default;

 private:
  virtual void setupPipeline() override;
  virtual void updatePipeline() override;

  int createFoliationBaseNoise(NoiseGraph* noise);
  int createFoliationWaveNoise(NoiseGraph* noise);
  int createMetamorhpicBaseNoise(NoiseGraph* noise);
  int createGrainsBaseNoise(NoiseGraph* noise);

  void updateTextureGenerator();
  void setParametersFromPreset();

  NoiseTextureGenerator* textureGenerator;

  FormGeneratorExtender formGeneratorExtender;
  TextureExtrasExtender textureExtrasExtender;

  bool usePreset = true;
  int presetChoice = 3;  // gneiss

  float baseGrainSize = 0.2;

  // foliation
  struct FoliationSettings {
    bool enabled = true;
    float size = 0.5;
    Eigen::Vector3f rotation = {0.0, 0.0, 0.0};
    float wavyness = 0.1;
    float scalyness = 0.1;
  } foliation;

  Eigen::Vector3f baseColor = {0.532, 0.355, 0.084};
  Eigen::Vector3f secondaryColor = {0.681, 0.681, 0.681};
  Eigen::Vector3f tertiaryColor = {0.0, 0.0, 0.0};
};
}  // namespace procrock
