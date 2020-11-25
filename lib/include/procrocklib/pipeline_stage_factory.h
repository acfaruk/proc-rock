#pragma once
#include <procrocklib/gen/all.h>
#include <procrocklib/mod/all.h>
#include <procrocklib/par/all.h>
#include <procrocklib/texadd/all.h>
#include <procrocklib/texgen/all.h>

#include <memory>

namespace procrock {

std::unique_ptr<Generator> inline createGeneratorFromId(unsigned int id) {
  switch (id) {
    case PipelineStage_Gen_Cuboid:
      return std::make_unique<CuboidGenerator>();
    case PipelineStage_Gen_Icosahedron:
      return std::make_unique<IcosahedronGenerator>();
    case PipelineStage_Gen_Pyramid:
      return std::make_unique<PyramidGenerator>();
    case PipelineStage_Gen_SkinSurface:
      return std::make_unique<SkinSurfaceGenerator>();
    default:
      assert(0 && "make sure all stages are handled!");
  }
}

std::unique_ptr<Modifier> inline createModifierFromId(unsigned int id) {
  switch (id) {
    case PipelineStage_Mod_Transform:
      return std::make_unique<TransformationModifier>();
    case PipelineStage_Mod_Subdivsion:
      return std::make_unique<SubdivisionModifier>();
    case PipelineStage_Mod_Decimate:
      return std::make_unique<DecimateModifier>();
    case PipelineStage_Mod_DisplaceAlongNormals:
      return std::make_unique<DisplaceAlongNormalsModifier>();
    case PipelineStage_Mod_CutPlane:
      return std::make_unique<CutPlaneModifier>();
    default:
      assert(0 && "make sure all stages are handled!");
  }
}

std::unique_ptr<Parameterizer> inline createParameterizerFromId(unsigned int id) {
  switch (id) {
    case PipelineStage_Par_LSCM:
      return std::make_unique<LSCM_Parameterizer>();
    case PipelineStage_Par_XATLAS:
      return std::make_unique<XAtlasParameterizer>();
    default:
      assert(0 && "make sure all stages are handled!");
  }
}

std::unique_ptr<TextureGenerator> inline createTextureGeneratorFromId(unsigned int id) {
  switch (id) {
    case PipelineStage_TexGen_Noise:
      return std::make_unique<NoiseTextureGenerator>();
    case PipelineStage_TexGen_Igneous:
      return std::make_unique<IgneousTextureGenerator>();
    default:
      assert(0 && "make sure all stages are handled!");
  }
}

std::unique_ptr<TextureAdder> inline createTextureAdderFromId(unsigned int id) {
  switch (id) {
    case PipelineStage_TexAdd_Noise:
      return std::make_unique<NoiseTextureAdder>();
    case PipelineStage_TexAdd_Cracks:
      return std::make_unique<CracksTextureAdder>();
    default:
      assert(0 && "make sure all stages are handled!");
  }
}
}  // namespace procrock