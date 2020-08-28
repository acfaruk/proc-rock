#pragma once
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

namespace procrock {
class Parameterizer : public PipelineStage {
 public:
  virtual std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

  virtual bool isMoveable() const override;
  virtual bool isRemovable() const override;

  inline bool isFirstRun() const { return firstRun; }

 protected:
  virtual std::shared_ptr<Mesh> parameterize(Mesh* mesh) = 0;

 private:
  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};

}  // namespace procrock