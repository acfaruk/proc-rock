#pragma once
#include <procrocklib/modifier.h>

namespace procrock {
class SubdivisionModifier : public Modifier {
 public:
  int subdivisions = 1;
  int mode = 0;

  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) override;
  Configuration getConfiguration() override;
  virtual PipelineStageInfo& getInfo() override;

 private:
  PipelineStageInfo info{
      PipelineStageNames_Mod[PipelineStage_Mod_Subdivsion],
      "Subdivides the mesh, to give it more geometry, optionally also making it more smooth"};
};
}  // namespace procrock