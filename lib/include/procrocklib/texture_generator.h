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

  // Maps position to height value
  typedef std::function<float(Eigen::Vector3f)> TextureFunction;

 protected:
  virtual std::shared_ptr<Mesh> generate(Mesh* before) = 0;

  void fillTexture(TextureGroup& texGroup, TextureFunction texFunction);

 private:
  static void fillPart(std::vector<float>& data, int startIndex, int endIndex,
                       const std::vector<TextureGroup::WorldMapEntry>& entries,
                       TextureFunction texFunction);
  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};

}  // namespace procrock