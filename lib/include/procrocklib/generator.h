#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

namespace procrock {

class Generator : public PipelineStage {
 public:
  virtual ~Generator() = default;

  std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

  virtual bool isMoveable() const override;
  virtual bool isRemovable() const override;

  inline bool isFirstRun() const { return firstRun; }

 protected:
  virtual std::shared_ptr<Mesh> generate() = 0;

 private:
  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};

}  // namespace procrock