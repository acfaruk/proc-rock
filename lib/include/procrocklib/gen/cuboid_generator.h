#pragma once
#include <procrocklib/generator.h>

namespace procrock {
class CuboidGenerator : public Generator {
 public:
  float length = 1.0f;
  float height = 0.5f;
  float depth = 0.2f;

  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Gen[PipelineStage_Gen_Cuboid],
                         "Generates a simple cuboid mesh based on width, length and depth"};
};
}  // namespace procrock