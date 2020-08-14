#pragma once
#include <procrocklib/modifier.h>

namespace procrock {
class SubdivisionModifier : public Modifier {
 public:
  int subdivisions = 1;

  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) override;
  Configuration getConfiguration() override;
};
}  // namespace procrock