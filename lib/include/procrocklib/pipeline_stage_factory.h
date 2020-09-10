#pragma once
#include <procrocklib/gen/all.h>
#include <procrocklib/mod/all.h>
#include <procrocklib/par/all.h>
#include <procrocklib/texgen/all.h>

#include <memory>

namespace procrock {

std::unique_ptr<Generator> inline createGeneratorFromId(unsigned int id) {
  switch (id) {
    case PipelineStage_Gen_Cuboid:
      return std::make_unique<CuboidGenerator>();
    case PipelineStage_Gen_Icosahedron:
      return std::make_unique<IcosahedronGenerator>();
    default:
      assert(0 && "make sure all stages are handled!");
  }
}

std::unique_ptr<Modifier> inline createModifierFromId(unsigned int id) {
  switch (id) {
    case PipelineStage_Mod_Subdivsion:
      return std::make_unique<SubdivisionModifier>();
    case PipelineStage_Mod_Decimate:
      return std::make_unique<DecimateModifier>();
    case PipelineStage_Mod_DisplaceAlongNormals:
      return std::make_unique<DisplaceAlongNormalsModifier>();
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
    case PipelineStage_TexGen_Checkerboard:
      return std::make_unique<CheckerboardTextureGenerator>();
    case PipelineStage_TexGen_SingleNoise:
      return std::make_unique<SingleNoiseTextureGenerator>();
    case PipelineStage_TexGen_CombinedNoise:
      return std::make_unique<CombinedNoiseTextureGenerator>();
    case PipelineStage_TexGen_SelectedNoise:
      return std::make_unique<SelectedNoiseTextureGenerator>();
    case PipelineStage_TexGen_Granite:
      return std::make_unique<GraniteTextureGenerator>();
    default:
      assert(0 && "make sure all stages are handled!");
  }
}
}  // namespace procrock