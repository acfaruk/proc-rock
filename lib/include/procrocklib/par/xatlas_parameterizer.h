#pragma once
#include <procrocklib/parameterizer.h>

namespace procrock {
class XAtlasParameterizer : public Parameterizer {
 public:
  XAtlasParameterizer();
  virtual ~XAtlasParameterizer() = default;

  struct ChartOptions {
    float normalDeviationWeight = 2.0f;
    float roundnessWeight = 0.01f;
    float straightnessWeight = 6.0f;
    float normalSeamWeight = 4.0f;
    float textureSeamWeight = 0.5f;

    float maxCost = 2.0f;
    int maxIterations = 1;
  } chartOptions;

  struct PackOptions {
    bool bilinear = true;
    bool blockAlign = true;
    bool bruteForce = false;
    int padding = 10;
  } packOptions;

  virtual PipelineStageInfo& getInfo() override;

 protected:
  virtual std::shared_ptr<Mesh> parameterize(Mesh* mesh) override;

 private:
  PipelineStageInfo info{PipelineStageNames_Par[PipelineStage_Par_XATLAS], "xatlas Library",
                         PipelineStageType::Parameterizer, PipelineStage_Par_XATLAS};
};
}  // namespace procrock