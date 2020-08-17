#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

namespace procrock {

class Generator : public PipelineStage {
 public:
  std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

 protected:
  virtual std::shared_ptr<Mesh> generate() = 0;

 private:
  std::shared_ptr<Mesh> mesh;
};
}  // namespace procrock