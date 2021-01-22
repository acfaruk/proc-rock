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
class IgneousPipeline : public AbstractedPipeline {
 public:
  IgneousPipeline();
  virtual ~IgneousPipeline() = default;

 private:
  virtual void setupPipeline() override;
  virtual void updatePipeline() override;

  void updateTextureGenerator();

  NoiseTextureGenerator* textureGenerator;

  FormGeneratorExtender formGeneratorExtender;
  TextureExtrasExtender textureExtrasExtender;

  int grainChoice = 0;  // 0 = fine grained, 1 = coarse, 2 = two distinct

  Eigen::Vector3f baseColor = {0.4, 0.2, 0.1};
  Eigen::Vector3f secondaryColor = {1.0, 1.0, 1.0};
  Eigen::Vector3f tertiaryColor = {0.0, 0.0, 0.0};
};
}  // namespace procrock
