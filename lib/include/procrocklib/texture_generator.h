#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

namespace procrock {

class TextureGenerator : public PipelineStage {
 public:
  float normalStrength = 1.0;

  std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

  virtual bool isMoveable() const override;
  virtual bool isRemovable() const override;

  inline bool isFirstRun() const { return firstRun; }

 protected:
  virtual std::shared_ptr<Mesh> generate(Mesh* before) = 0;

  virtual Configuration getBaseConfiguration() override;

  void fillTexture(const TextureGroup& texGroup, std::vector<unsigned char>& dataToFill,
                   std::function<Eigen::Vector3i(Eigen::Vector3d)> colorFunction);

  void calculateNormals(const TextureGroup& texGroup, std::vector<unsigned char>& dataToFill);

 private:
  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};

}  // namespace procrock