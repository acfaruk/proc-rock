#pragma once
#include <procrocklib/generator.h>

namespace procrock {
class CuboidGenerator : public Generator {
 public:
  float length = 1.0f;
  float height = 0.5f;
  float depth = 0.2f;

  virtual Configuration getConfiguration() override;

 protected:
  std::shared_ptr<Mesh> generate() override;
};
}  // namespace procrock