#pragma once
#include <procrocklib/gen/all.h>
#include <procrocklib/mod/all.h>

#include "../abstracted_pipeline_extender.h"

namespace procrock {
class FormGeneratorExtender : public AbstractedPipelineExtender {
 public:
  using AbstractedPipelineExtender::AbstractedPipelineExtender;

  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void setupPipeline(Pipeline* pipeline) override;
  virtual void updatePipeline(Pipeline* pipeline) override;

 private:
  void updateGeometryRoughness();

  int formChoice = 1;  // 0 = pebble, 1 = liquid, 2 = cube, 3 = boulder
  int seed = 0;
  bool cutGround = true;
  float geometryDetail = 1.0;

  float roughness = 0.2;
  float distortion = 0.15;

  IcosahedronGenerator* genPebble;
  SkinSurfaceGenerator* genLiquid;
  CuboidGenerator* genCube;
  PyramidGenerator* genBoulder;

  CutPlaneModifier* modPyramidCut;
  SubdivisionModifier* modSubdiv;
  DisplaceAlongNormalsModifier* modRoughness;
  CutPlaneModifier* modGroundCut;
  TransformationModifier* modTransform;
  DecimateModifier* modDecimate;
  DecimateModifier* modSkinDecimate;
};
}  // namespace procrock