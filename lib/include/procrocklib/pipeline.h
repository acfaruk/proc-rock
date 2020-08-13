#pragma once
#include <procrocklib/generator.h>

#include <memory>

namespace procrock {
class Pipeline {
 public:
  void setGenerator(std::unique_ptr<Generator> generator);
  Generator& getGenerator() const;

  const std::shared_ptr<Mesh> getCurrentMesh();

 private:
  std::unique_ptr<Generator> generator;

  std::shared_ptr<Mesh> currentMesh;
};
}  // namespace procrock