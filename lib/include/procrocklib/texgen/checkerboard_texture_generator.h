#pragma once
#include <procrocklib/texture_generator.h>

namespace procrock {
class CheckerboardTextureGenerator : public TextureGenerator {
 public:
  CheckerboardTextureGenerator();

  int squares = 10;
  int mode = 0;

  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexGen[PipelineStage_TexGen_Checkerboard],
                         "Generates a simple checkerboard texture to test the parameterization.",
                         PipelineStageType::TextureGenerator, PipelineStage_TexGen_Checkerboard};

  NormalsGenerator normalsGenerator;
  RoughnessGenerator roughnessGenerator;
  MetalnessGenerator metalnessGenerator;
  AmbientOcclusionGenerator ambientOccGenerator;
};
}  // namespace procrock