#pragma once
#include <procrocklib/texadd/all.h>

#include "../abstracted_pipeline_extender.h"

namespace procrock {
class TextureExtrasExtender : public AbstractedPipelineExtender {
 public:
  using AbstractedPipelineExtender::AbstractedPipelineExtender;

  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void setupPipeline(Pipeline* pipeline) override;
  virtual void updatePipeline(Pipeline* pipeline) override;

 private:
  void updateTextureAdderVariance();
  void updateTextureAdderMoss();

  bool textureVariance = true;
  bool textureMoss = false;

  NoiseTextureAdder* textureAdderVariance;
  NoiseTextureAdder* textureAdderMoss;
};
}  // namespace procrock