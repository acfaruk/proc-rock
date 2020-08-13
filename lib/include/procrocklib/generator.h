#pragma once
#include <procrocklib/mesh.h>

#include <memory>

#include "configurable.h"

namespace procrock {
class Generator : public Configurable {
 public:
  virtual std::shared_ptr<Mesh> generate() = 0;
};
}  // namespace procrock