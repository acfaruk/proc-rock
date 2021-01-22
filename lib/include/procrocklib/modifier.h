#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

namespace procrock {
class Modifier : public PipelineStage, public Disablable {
 public:
  virtual ~Modifier() = default;
  std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

  virtual bool isMoveable() const override;
  virtual bool isRemovable() const override;

  inline bool isFirstRun() const { return firstRun; }

 protected:
  virtual std::shared_ptr<Mesh> modify(Mesh& mesh) = 0;

 private:
  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};

}  // namespace procrock