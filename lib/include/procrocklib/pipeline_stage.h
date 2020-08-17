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

  inline std::string getId() {
    // Each instance gets its id from its name and memory location
    std::ostringstream oss;
    oss << (void*)this;
    return getInfo().name + "(" + oss.str() + ")";
  }
};

static const char* PipelineStage_Gen_Cuboid = "Cuboid Generator";

static const char* PipelineStage_Mod_Subdivsion = "Subdivision Modifier";  // 0

static const char* const PipelineStage_Mod_All[] = {PipelineStage_Mod_Subdivsion};

}  // namespace procrock