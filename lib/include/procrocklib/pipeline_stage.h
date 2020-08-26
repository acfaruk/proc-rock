#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/mesh.h>

#include <memory>

namespace procrock {

struct PipelineStageInfo {
  std::string name = "Default PipelineStage";
  std::string description = "-";
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
    return getInfo().name + "(" + oss.str() + ")";
  }
};

static const char* const PipelineStage_Gen_Cuboid = "Cuboid Generator";

static const char* const PipelineStage_Mod_Subdivsion = "Subdivision Modifier";              // 0
static const char* const PipelineStage_Mod_DisplaceAlongNormals = "Displace Along Normals";  // 1

static const char* const PipelineStage_Par_LSCM = "LSCM Parametrization";
static const char* const PipelineStage_Par_XATLAS = "XATLAS Parametrization";

static const char* const PipelineStage_TexGen_Checkerboard = "Checkerboard Generator";

static const char* const PipelineStage_Mod_All[] = {PipelineStage_Mod_Subdivsion,
                                                    PipelineStage_Mod_DisplaceAlongNormals};

}  // namespace procrock