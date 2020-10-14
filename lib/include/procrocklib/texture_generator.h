#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/configurables/texturing.h>
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

namespace procrock {

class TextureGenerator : public PipelineStage {
 public:
  TextureGenerator();

  std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

  virtual bool isMoveable() const override;
  virtual bool isRemovable() const override;

  inline bool isFirstRun() const { return firstRun; }

 protected:
  virtual std::shared_ptr<Mesh> generate(Mesh* before) = 0;

  void fillTexture(TextureGroup& texGroup,
                   std::function<Eigen::Vector3i(Eigen::Vector3d)> colorFunction);

 private:
  static void fillPart(std::vector<unsigned char>& data, int startIndex, int endIndex,
                       const std::vector<TextureGroup::WorldMapEntry>& entries,
                       std::function<Eigen::Vector3i(Eigen::Vector3d)> colorFunction);
  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};

}  // namespace procrock