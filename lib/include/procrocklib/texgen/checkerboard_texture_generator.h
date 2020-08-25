#pragma once
#include <procrocklib/texture_generator.h>

namespace procrock {
class CheckerboardTextureGenerator : public TextureGenerator {
 public:
  int squares = 10;
  int mode = 0;

  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStage_TexGen_Checkerboard,
                         "Generates a simple checkerboard texture to test the parameterization."};
};
}  // namespace procrock