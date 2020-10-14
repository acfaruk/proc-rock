#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/configurables/texturing.h>
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

namespace procrock {

class TextureAdder : public PipelineStage {
 public:
  TextureAdder(bool hideConfigurables = false);

  std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

  virtual bool isMoveable() const override;
  virtual bool isRemovable() const override;

  inline bool isFirstRun() const { return firstRun; }

 protected:
  virtual std::shared_ptr<Mesh> generate(Mesh* before) = 0;

  TextureGroup createAddTexture(Mesh& mesh,
                                std::function<Eigen::Vector4i(Eigen::Vector3f)> colorFunction);
  void addTextures(Mesh& mesh, TextureGroup& addGroup);

 protected:
  float normalProportion = 0.01;
  float roughnessProportion = 0.1;
  float metalProportion = 1.0;
  float ambientOccProportion = 0.5;

  struct PreferredNormalDirectionStruct {
    bool enabled = false;
    Eigen::Vector3f direction{0, 1, 0};
    float strength = 1.0f;
  } preferred;

 private:
  static void fillPart(std::vector<unsigned char>& data, int startIndex, int endIndex,
                       const Mesh& mesh,
                       std::function<Eigen::Vector4i(Eigen::Vector3f)> colorFunction,
                       PreferredNormalDirectionStruct preferred);

  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};

}  // namespace procrock