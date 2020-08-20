#pragma once
#include <procrocklib/generator.h>
#include <procrocklib/modifier.h>
#include <procrocklib/parameterizer.h>

#include <memory>

namespace procrock {
class Generator;

class Pipeline {
 public:
  void setGenerator(std::unique_ptr<Generator> generator);
  Generator& getGenerator() const;

  int getModifierCount();
  void addModifier(std::unique_ptr<Modifier> modifier);
  void addModifierFromId(unsigned int id);
  void movePipelineStageUp(PipelineStage* stage);
  void movePipelineStageDown(PipelineStage* stage);
  Modifier& getModifier(int index);

  void setParameterizer(std::unique_ptr<Parameterizer> parameterizer);
  Parameterizer& getParameterizer() const;

  void removePipelineStage(PipelineStage* stage);

  const std::shared_ptr<Mesh> getCurrentMesh();

 private:
  std::unique_ptr<Generator> generator;
  std::vector<std::unique_ptr<Modifier>> modifiers;
  std::unique_ptr<Parameterizer> parameterizer;

  std::shared_ptr<Mesh> currentMesh;
};
}  // namespace procrock