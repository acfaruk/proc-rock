#pragma once

#include <procrocklib/texture_generator.h>

namespace procrock {
class GraniteTextureGenerator : public TextureGenerator {
 public:
  float frequency = 40.0f;
  float lacunarity = 2.18359375f;
  float persistence = 0.625f;
  float grainScaling = -0.5f;
  int roughness = 6;
  int octaveCount = 6;
  int seed = 0;

  int qualityChoice = 2;

  std::map<int, Eigen::Vector3f> colorGradient{{0, {0, 0, 0}},
                                               {3, {0, 0, 0}},
                                               {6, {0.847, 0.847, 0.949}},
                                               {50, {0.749, 0.749, 0.749}},
                                               {75, {0.824, 0.455, 0.490}},
                                               {88, {0.824, 0.443, 0.384}},
                                               {100, {1.000, 0.690f, 0.753f}}};

  virtual Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 protected:
  std::shared_ptr<Mesh> generate(Mesh* before) override;

 private:
  PipelineStageInfo info{PipelineStageNames_TexGen[PipelineStage_TexGen_Granite],
                         "Generate a texture resembling granite."};
};
}  // namespace procrock