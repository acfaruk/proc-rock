#pragma once

#include <procrocklib/generator.h>

namespace procrock {
class IcosahedronGenerator : public Generator {
 public:
  IcosahedronGenerator();
  virtual ~IcosahedronGenerator() = default;

  float edgeLength = 1;

  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Gen[PipelineStage_Gen_Icosahedron],
                         "Generates a simple icosahedron mesh based on edge length.",
                         PipelineStageType::Generator, PipelineStage_Gen_Icosahedron};
};
}  // namespace procrock