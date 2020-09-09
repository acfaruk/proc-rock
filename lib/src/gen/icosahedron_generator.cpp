#include "gen/icosahedron_generator.h"

#include <igl/per_vertex_normals.h>

namespace procrock {

std::shared_ptr<Mesh> IcosahedronGenerator::generate() {
  Eigen::Matrix<double, 12, 3> vertices;

  float x = 0.5f * edgeLength;
  float z = 0.809f * edgeLength;
  float n = 0.0f;

  vertices << -x, n, z, x, n, z, -x, n, -z, x, n, -z, n, z, x, n, z, -x, n, -z, x, n, -z, -x, z, x,
      n, -z, x, n, z, -x, n, -z, -x, n;

  Eigen::Matrix<int, 20, 3> faces;

  faces << 1, 4, 0, 4, 9, 0, 4, 5, 9, 8, 5, 4, 1, 8, 4, 1, 10, 8, 10, 3, 8, 8, 3, 5, 3, 2, 5, 3, 7,
      2, 3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6, 1, 0, 10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2, 9, 11,
      2, 7;

  auto result = std::make_shared<Mesh>();
  result->vertices = vertices;
  result->faces = faces;
  igl::per_vertex_normals(result->vertices, result->faces, result->normals);
  return result;
}

Configuration IcosahedronGenerator::getConfiguration() {
  Configuration::ConfigurationGroup group;
  group.entry = {"General Settings", "Set various parameters of the icosahedron."};

  group.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Edge length", "Length of a single edge of the Icosahedron"}, &edgeLength, 0.1, 1.0f});

  Configuration result;
  result.insertToConfigGroups("General", group);
  return result;
}
PipelineStageInfo& IcosahedronGenerator::getInfo() { return info; }

}  // namespace procrock