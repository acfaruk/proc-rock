#pragma once
#include <procrocklib/generator.h>
#include <procrocklib/modifier.h>

#include <memory>

namespace procrock {
class Generator;

class Pipeline {
 public:
  void setGenerator(std::unique_ptr<Generator> generator);
  Generator& getGenerator() const;

  int getModifierCount();
  void addModifier(std::unique_ptr<Modifier> modifier);
  Modifier& getModifier(int index);

  const std::shared_ptr<Mesh> getCurrentMesh();

 private:
  std::unique_ptr<Generator> generator;
  std::vector<std::unique_ptr<Modifier>> modifiers;

  std::shared_ptr<Mesh> currentMesh;
};
}  // namespace procrock