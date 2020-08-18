#include "mod/displace_along_normals_modifier.h"

#include <igl/bfs_orient.h>
#include <igl/per_vertex_normals.h>

#include <Eigen/Geometry>

namespace procrock {
std::shared_ptr<Mesh> DisplaceAlongNormalsModifier::modify(Mesh& mesh) {
  std::shared_ptr<Mesh> result = std::make_shared<Mesh>();
  result->vertices.resize(mesh.vertices.rows(), mesh.vertices.cols());
  result->faces = mesh.faces;

  rng.seed(seed);
  for (Eigen::Index i = 0; i < mesh.vertices.rows(); i++) {
    float random = rng() / (float)rng.max();
    auto translation =
        Eigen::Translation<double, 3>((factor * random * mesh.normals.row(i).normalized()));
    auto transformedPos = translation * Eigen::Vector3d(mesh.vertices.row(i));
    result->vertices.row(i) = transformedPos;
  }
  igl::per_vertex_normals(result->vertices, result->faces, result->normals);

  return result;
}
Configuration DisplaceAlongNormalsModifier::getConfiguration() {
  Configuration result;

  result.ints.emplace_back(
      Configuration::BoundedEntry<int>{{"Seed", "Seed for the rng"}, &seed, 0, 100000});

  result.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Factor", "Amount of displacement"}, &factor, 0.001, 0.2});
  return result;
}
PipelineStageInfo& DisplaceAlongNormalsModifier::getInfo() { return info; }
}  // namespace procrock