#pragma once
#include <procrocklib/mesh.h>

#include <memory>

namespace procrock {

class PipelineStage {
 public:
  virtual std::shared_ptr<Mesh> run(Mesh* before = nullptr) = 0;
};
}  // namespace procrock