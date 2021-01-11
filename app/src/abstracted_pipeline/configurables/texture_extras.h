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
  void updateTextureAdderSpots();
  void updateTextureAdderVeins();
  void updateTextureAdderMoss();

  bool textureVariance = true;
  bool textureMoss = false;
  bool textureVeins = false;
  bool textureSpots = false;

  struct VeinSettings {
    bool bigVeins = true;
    bool smallVeins = true;
    float size = 1;
    Eigen::Vector3f color = {0, 0, 0};
  } veins;

  struct SpotSettings {
    float size = 1;
    float strength = 0.3;
    Eigen::Vector3f color = {0.35, 0.35, 0};
  } spots;

  int textureSizeChoice = 2;

  NoiseTextureAdder* textureAdderVariance;
  NoiseTextureAdder* textureAdderSpots;
  NoiseTextureAdder* textureAdderVeins;
  NoiseTextureAdder* textureAdderMoss;
};
}  // namespace procrock