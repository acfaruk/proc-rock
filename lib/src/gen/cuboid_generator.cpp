#include "gen/cuboid_generator.h"

#include "igl/per_vertex_normals.h"

namespace procrock {
std::shared_ptr<Mesh> CuboidGenerator::generate() {
  Eigen::Matrix<double, 8, 3> vertices;

  double half_length = length / 2.0;
  double half_height = height / 2.0;
  double half_depth = depth / 2.0;

  vertices.row(0) << half_length, -half_height, -half_depth;
  vertices.row(1) << half_length, half_height, -half_depth;
  vertices.row(2) << -half_length, half_height, -half_depth;
  vertices.row(3) << -half_length, -half_height, -half_depth;

  vertices.row(4) << half_length, -half_height, half_depth;
  vertices.row(5) << half_length, half_height, half_depth;
  vertices.row(6) << -half_length, half_height, half_depth;
  vertices.row(7) << -half_length, -half_height, half_depth;

  Eigen::Matrix<int, 12, 3> faces;

  faces.row(0) << 0, 2, 1;
  faces.row(1) << 0, 3, 2;

  faces.row(2) << 0, 1, 5;
  faces.row(3) << 0, 5, 4;

  faces.row(4) << 1, 2, 6;
  faces.row(5) << 1, 6, 5;

  faces.row(6) << 2, 3, 7;
  faces.row(7) << 2, 7, 6;

  faces.row(8) << 3, 0, 4;
  faces.row(9) << 3, 4, 7;

  faces.row(10) << 4, 5, 6;
  faces.row(11) << 4, 6, 7;

  auto result = std::make_shared<Mesh>();
  result->vertices = vertices;
  result->faces = faces;
  igl::per_vertex_normals(result->vertices, result->faces, result->normals);
  return result;
}

Configuration CuboidGenerator::getConfiguration() {
  Configuration::ConfigurationGroup group;
  group.entry = {"General Settings", "Set various parameters of the cuboid."};
  group.floats.emplace_back(
      Configuration::BoundedEntry<float>{"Length", "Length of the Cuboid", &length, 0.1f, 1.0f});
  group.floats.emplace_back(
      Configuration::BoundedEntry<float>{"Height", "Height of the Cuboid", &height, 0.1f, 1.0f});
  group.floats.emplace_back(
      Configuration::BoundedEntry<float>{"Depth ", "Depth of the Cuboid", &depth, 0.1f, 1.0f});

  Configuration result;
  result.insertToConfigGroups("General", group);
  return result;
}
PipelineStageInfo& CuboidGenerator::getInfo() { return info; }

}  // namespace procrock
