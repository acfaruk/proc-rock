#pragma once
#include <procrocklib/generator.h>

namespace procrock {
class CuboidGenerator : public Generator {
 public:
  CuboidGenerator();
  virtual ~CuboidGenerator() = default;

  float length = 1.0f;
  float height = 0.5f;
  float depth = 0.2f;

  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Gen[PipelineStage_Gen_Cuboid],
                         "Generates a simple cuboid mesh based on width, length and depth",
                         PipelineStageType::Generator, PipelineStage_Gen_Cuboid};
};
}  // namespace procrock