#pragma once
#include <procrocklib/texture_generator.h>

namespace procrock {
class PerlinNoiseTextureGenerator : public TextureGenerator {
 public:
  float frequency = 9.0f;
  float lacunarity = 1.5f;
  float persistence = 0.5f;
  int octaveCount = 3;
  int seed = 0;

  int qualityChoice = 2;

  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexGen[PipelineStage_TexGen_PerlinNoise],
                         "Generates Solid 3D Perlin Noise."};
};
}  // namespace procrock