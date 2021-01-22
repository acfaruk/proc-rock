#pragma once
#include <procrocklib/configurables/noise_graph.h>
#include <procrocklib/modifier.h>

#include <random>
#include <set>

namespace procrock {
class DisplaceAlongNormalsModifier : public Modifier {
 public:
  DisplaceAlongNormalsModifier();
  virtual ~DisplaceAlongNormalsModifier() = default;

  int selection = 0;  // 0 = rng, 1 = noise graph
  int seed = 0;
  // int ignoredVerticesCount = 0;
  float factor = 0.001;

  bool preferDirection = false;
  float preferStrength = 1.0f;
  Eigen::Vector3f preferredDirection{0, -1, 0};

  NoiseGraph noiseGraph;

  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) override;
  virtual PipelineStageInfo& getInfo() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Mod[PipelineStage_Mod_DisplaceAlongNormals],
                         "Displace all vertices allong their respective normals",
                         PipelineStageType::Modifier, PipelineStage_Mod_DisplaceAlongNormals};

  std::mt19937 rng;
  int vertexCount = 0;
};
}  // namespace procrock