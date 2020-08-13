#pragma once
#include <procrocklib/generator.h>

namespace procrock {
class CuboidGenerator : public Generator {
 public:
  float length = 1.0f;
  float height = 0.5f;
  float depth = 0.2f;

  std::shared_ptr<Mesh> generate() override;
  virtual Configuration getConfiguration() override;

 private:
  std::shared_ptr<Mesh> mesh;
};
}  // namespace procrock