#pragma once
#include <procrocklib/generator.h>
#include <procrocklib/modifier.h>
#include <procrocklib/parameterizer.h>
#include <procrocklib/texture_adder.h>
#include <procrocklib/texture_generator.h>

#include <iostream>
#include <memory>

namespace procrock {
class Generator;

class Pipeline {
 public:
  void setGenerator(std::unique_ptr<Generator> generator);
  Generator& getGenerator() const;

  int getModifierCount();
  void addModifier(std::unique_ptr<Modifier> modifier);
  void movePipelineStageUp(PipelineStage* stage);
  void movePipelineStageDown(PipelineStage* stage);
  Modifier& getModifier(int index);

  void setParameterizer(std::unique_ptr<Parameterizer> parameterizer);
  Parameterizer& getParameterizer() const;

  void setTextureGenerator(std::unique_ptr<TextureGenerator> textureGenerator);
  TextureGenerator& getTextureGenerator() const;

  int getTextureAdderCount();
  void addTextureAdder(std::unique_ptr<TextureAdder> textureAdder);
  TextureAdder& getTextureAdder(int index);

  void removePipelineStage(PipelineStage* stage);

  const std::shared_ptr<Mesh> getCurrentMesh();

  void clear();

  bool isChanged();

  void enableOutput(bool enable);
  void setOutputStream(std::ostream* stream);

  void saveToFile(const std::string filePath);
  void loadFromFile(const std::string filePath);

  struct ExportSettings {
    bool exportLODs = false;
    int lodCount = 3;
    bool lodTextures = false;

    bool exportAlbedo = true;
    bool exportNormals = true;
    bool exportRoughness = true;
    bool exportMetal = true;
    bool exportDisplacement = true;
    bool exportAmbientOcc = true;
  };

  void exportCurrent(const std::string filePath, ExportSettings settings);

 private:
  std::unique_ptr<Generator> generator;
  std::vector<std::unique_ptr<Modifier>> modifiers;
  std::unique_ptr<Parameterizer> parameterizer;
  std::unique_ptr<TextureGenerator> textureGenerator;
  std::vector<std::unique_ptr<TextureAdder>> textureAdders;

  std::shared_ptr<Mesh> currentMesh;

  bool outputEnabled = true;
  std::ostream* outputStream = &std::cout;
};
}  // namespace procrock