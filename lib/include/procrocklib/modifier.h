#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

namespace procrock {
class Modifier : public PipelineStage {
 public:
  std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

 protected:
  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) = 0;

 private:
  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};
}  // namespace procrock