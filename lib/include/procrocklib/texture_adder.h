#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/configurables/texturing.h>
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

namespace procrock {

class TextureAdder : public PipelineStage {
 public:
  std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

  virtual bool isMoveable() const override;
  virtual bool isRemovable() const override;

  inline bool isFirstRun() const { return firstRun; }

 protected:
  virtual std::shared_ptr<Mesh> generate(Mesh* before) = 0;

  virtual Configuration getBaseConfiguration() override;

  void addTexture(TextureGroup& texGroup,
                  std::function<Eigen::Vector4i(Eigen::Vector3d)> colorFunction);

 private:
  NormalsGenerator normalsGenerator;
  RoughnessGenerator roughnessGenerator;
  MetalnessGenerator metalnessGenerator;
  AmbientOcclusionGenerator ambientOccGenerator;

  float normalProportion = 0.01;
  float roughnessProportion = 0.1;
  float metalProportion = 1.0;
  float ambientOccProportion = 0.5;

  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};

}  // namespace procrock