#pragma once
#include <procrocklib/generator.h>

#include <random>

namespace procrock {
class PyramidGenerator : public Generator {
 public:
  PyramidGenerator();
  virtual ~PyramidGenerator() = default;

  float height = 2;
  float tipX = 0;
  float tipZ = 0;

  float baseHeight = -1;
  float baseRadius = 2;
  int baseSubdivisions = 4;
  float baseWiggleAmount = 0;

  int seed = 0;

  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Gen[PipelineStage_Gen_Pyramid],
                         "Generates a simple pyramid mesh.", PipelineStageType::Generator,
                         PipelineStage_Gen_Pyramid};
  std::mt19937 rng;
};
}  // namespace procrock