#pragma once
#include <procrocklib/gen/skin_surface_generator.h>

#include "abstracted_pipeline.h"

namespace procrock {
class IgneousPipeline : public AbstractedPipeline {
 public:
  IgneousPipeline();

 private:
  virtual void setupPipeline() override;
  virtual void updatePipeline() override;

  SkinSurfaceGenerator* generator;
  int seed = 0;
};
}  // namespace procrock
