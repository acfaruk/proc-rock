#pragma once
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

#include "configurable.h"

namespace procrock {

class Generator : public Configurable, PipelineStage {
 public:
  virtual std::shared_ptr<Mesh> getMesh();

 protected:
  virtual std::shared_ptr<Mesh> generate() = 0;

 private:
  std::shared_ptr<Mesh> mesh;
};
}  // namespace procrock