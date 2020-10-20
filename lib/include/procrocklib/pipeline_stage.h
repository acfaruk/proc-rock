#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/mesh.h>

#include <memory>

namespace procrock {

enum class PipelineStageType {
  NoType,
  Generator,
  Modifier,
  Parameterizer,
  TextureGenerator,
  TextureAdder
};

struct PipelineStageInfo {
  std::string name = "Default PipelineStage";
  std::string description = "-";
  PipelineStageType type = PipelineStageType::NoType;
  int id = -1;
};

class PipelineStage : public Configurable {
 public:
  virtual std::shared_ptr<Mesh> run(Mesh* before = nullptr) = 0;
  virtual PipelineStageInfo& getInfo() = 0;

  virtual bool isMoveable() const = 0;
  virtual bool isRemovable() const = 0;

  inline std::string getId() {
    // Each instance gets its id from its name and memory location
    std::ostringstream oss;
    oss << (void*)this;
    return getInfo().name + "##(" + oss.str() + ")";
  }
};

// Generators
const unsigned int PipelineStage_Gen_Cuboid = 0;
const unsigned int PipelineStage_Gen_Icosahedron = 1;
const unsigned int PipelineStage_Gen_Pyramid = 2;

const char* const PipelineStageNames_Gen[] = {"Cuboid", "Icosahedron", "Pyramid"};

// Modifiers
const unsigned int PipelineStage_Mod_Transform = 0;
const unsigned int PipelineStage_Mod_Subdivsion = 1;
const unsigned int PipelineStage_Mod_Decimate = 2;
const unsigned int PipelineStage_Mod_DisplaceAlongNormals = 3;

const char* const PipelineStageNames_Mod[] = {"Transform", "Subdivision", "Decimation",
                                              "Displace Along Normals"};

// Parameterizers
const unsigned int PipelineStage_Par_LSCM = 0;
const unsigned int PipelineStage_Par_XATLAS = 1;

const char* const PipelineStageNames_Par[] = {"LSCM Parameterizer", "xatlas Parameterizer"};

// Texture Generators
const unsigned int PipelineStage_TexGen_Checkerboard = 0;
const unsigned int PipelineStage_TexGen_Noise = 1;
const unsigned int PipelineStage_TexGen_Igneous = 2;

const char* const PipelineStageNames_TexGen[] = {"Checkerboard", "Noise", "Igneous"};

// Texture Adders
const unsigned int PipelineStage_TexAdd_Noise = 0;
const unsigned int PipelineStage_TexAdd_Cracks = 1;

const char* const PipelineStageNames_TexAdd[] = {"Noise", "Cracks"};

}  // namespace procrock