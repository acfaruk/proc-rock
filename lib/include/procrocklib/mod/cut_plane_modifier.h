#pragma once
#include <procrocklib/modifier.h>

namespace procrock {
class CutPlaneModifier : public Modifier {
 public:
  CutPlaneModifier();

  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) override;
  virtual PipelineStageInfo& getInfo() override;

 private:
  PipelineStageInfo info{PipelineStageNames_Mod[PipelineStage_Mod_CutPlane],
                         "Cut the mesh along a plane.", PipelineStageType::Modifier,
                         PipelineStage_Mod_CutPlane};

  void convert(Eigen::MatrixXf vertices, std::vector<float>& vOut);
  void convert(Eigen::MatrixXi faces, std::vector<unsigned int>& fOut);
};
}  // namespace procrock