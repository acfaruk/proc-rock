#pragma once
#include <procrocklib/mesh.h>

#include <memory>

namespace procrock {

class PipelineStage {
  virtual std::shared_ptr<Mesh> getMesh() = 0;
};
}  // namespace procrock