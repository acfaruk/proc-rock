#pragma once
#include <procrocklib/texture_generator.h>

namespace procrock {
class PerlinNoiseTextureGenerator : public TextureGenerator {
 public:
  float frequency = 31.0f;
  float lacunarity = 2.5f;
  float persistence = 0.6f;
  int octaveCount = 3;
  int seed = 0;

  int qualityChoice = 2;

  std::map<int, Eigen::Vector3f> colorGradient{{0, {0.827, 0.784, 0.517}},
                                               {30, {0.286, 0.225, 0.225}},
                                               {45, {0.427, 0.395, 0.395}},
                                               {65, {0.569, 0.553, 0.378}},
                                               {100, {0.940, 0.936, 0.921}}};

  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexGen[PipelineStage_TexGen_PerlinNoise],
                         "Generates Solid 3D Perlin Noise."};
};
}  // namespace procrock