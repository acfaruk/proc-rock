#pragma once
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

namespace procrock {
class Parameterizer : public PipelineStage {
 public:
  int textureSizeChoice = 1;

  virtual std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

  virtual bool isMoveable() const override;
  virtual bool isRemovable() const override;

  inline bool isFirstRun() const { return firstRun; }
  virtual Configuration getBaseConfiguration() override;

 protected:
  virtual std::shared_ptr<Mesh> parameterize(Mesh* mesh) = 0;
  void setTextureGroupSize(Mesh& mesh);
  void fillTextureMapFaceBased(Mesh& mesh);

 private:
  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};

}  // namespace procrock