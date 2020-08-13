#include "gen/cuboid_generator.h"

#include "igl/per_vertex_normals.h"

namespace procrock {
std::shared_ptr<Mesh> CuboidGenerator::generate() {
  Eigen::Matrix<float, 8, 3> vertices;

  float half_length = length / 2.0f;
  float half_height = height / 2.0f;
  float half_depth = depth / 2.0f;

  vertices.row(0) << half_length, -half_depth, -half_height;
  vertices.row(1) << half_length, half_depth, -half_height;
  vertices.row(2) << -half_length, half_depth, -half_height;
  vertices.row(3) << -half_length, -half_depth, -half_height;

  vertices.row(4) << half_length, -half_depth, half_height;
  vertices.row(5) << half_length, half_depth, half_height;
  vertices.row(6) << -half_length, half_depth, half_height;
  vertices.row(7) << -half_length, -half_depth, half_height;

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
  igl::per_vertex_normals(vertices, faces, result->normals);
  return result;
}

Configuration CuboidGenerator::getConfiguration() {
  Configuration result;

  result.floats.emplace_back(
      Configuration::Entry<float>{"Length", "Length of the Cuboid to be generated", &length});
  result.floats.emplace_back(
      Configuration::Entry<float>{"Height", "Height of the Cuboid to be generated", &height});
  result.floats.emplace_back(
      Configuration::Entry<float>{"Depth ", "Depth of the Cuboid to be generated", &depth});
  return result;
}
}  // namespace procrock
