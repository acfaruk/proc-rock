#pragma once
#include <procrocklib/configurable.h>
#include <procrocklib/configurables/texturing.h>
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

namespace procrock {

class TextureAdder : public PipelineStage, public Disablable {
 public:
  TextureAdder(bool hideConfigurables = false);
  virtual ~TextureAdder() = default;

  std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

  virtual bool isMoveable() const override;
  virtual bool isRemovable() const override;

  inline bool isFirstRun() const { return firstRun; }

 protected:
  // Maps position to height value
  typedef std::function<float(Eigen::Vector3f)> TextureFunction;

  virtual std::shared_ptr<Mesh> generate(Mesh* before) = 0;

  TextureGroup createAddTexture(Mesh& mesh, TextureFunction texFunction);
  void addTextures(Mesh& mesh, TextureGroup& addGroup);

 protected:
  float displacementProportion = 0.5;
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
  static void fillPart(std::vector<float>& data, int startIndex, int endIndex, const Mesh& mesh,
                       TextureFunction texFunction, PreferredNormalDirectionStruct preferred);

  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
};

}  // namespace procrock