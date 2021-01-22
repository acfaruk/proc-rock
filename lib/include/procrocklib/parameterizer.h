#pragma once
#include <procrocklib/mesh.h>
#include <procrocklib/pipeline_stage.h>

#include <memory>

namespace procrock {
class Parameterizer : public PipelineStage {
 public:
  Parameterizer();
  virtual ~Parameterizer() = default;

  int textureSizeChoice = 2;

  virtual std::shared_ptr<Mesh> run(Mesh* before = nullptr) override;

  virtual bool isMoveable() const override;
  virtual bool isRemovable() const override;

  inline bool isFirstRun() const { return firstRun; }

 protected:
  virtual std::shared_ptr<Mesh> parameterize(Mesh* mesh) = 0;
  void setTextureGroupSize(Mesh& mesh);
  void fillTextureMapFaceBased(Mesh& mesh);

 private:
  std::shared_ptr<Mesh> mesh;
  bool firstRun = true;
  struct TextureMapPatch {
    int face;
    Eigen::Vector3f faceTangent;
    int x, y, height, width;
    std::vector<TextureGroup::WorldMapEntry> worldMap;
  };

  static void fillTextureMapPatches(int startIndex, int endIndex,
                                    std::vector<TextureMapPatch>& result, const Mesh& mesh);

  static void fillTextureMapPatch(TextureMapPatch& patch, const Mesh& mesh);
  void applyTextureMapPatches(Mesh& mesh, const std::vector<TextureMapPatch>& patches);
};

}  // namespace procrock