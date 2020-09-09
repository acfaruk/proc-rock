#pragma once
#include <procrocklib/configurables/noise.h>
#include <procrocklib/modifier.h>

#include <random>
#include <set>

namespace procrock {
class DisplaceAlongNormalsModifier : public Modifier {
 public:
  int selection = 0;  // 0 = rng, 1 = single Noise, 2 = combined noise, 3 = selected noise
  int seed = 0;
  int ignoredVerticesCount = 0;
  float factor = 0.001;

  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) override;
  Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Mod[PipelineStage_Mod_DisplaceAlongNormals],
                         "Displace all vertices allong their respective normals"};

  std::mt19937 rng;
  SingleNoiseModule singleNoiseModule;
  CombinedNoiseModule combinedNoiseModule;
  SelectedNoiseModule selectedNoiseModule;

  int vertexCount = 0;

  std::set<int> pickSet(int N, int k);
};
}  // namespace procrock