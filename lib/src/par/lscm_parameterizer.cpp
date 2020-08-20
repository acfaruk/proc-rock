#include "par/lscm_parameterizer.h"

#include <igl/lscm.h>

namespace procrock {
std::shared_ptr<Mesh> LSCM_Parameterizer::parameterize(Mesh* mesh) {
  auto result = std::make_shared<Mesh>();
  indexMax = mesh->vertices.rows() - 1;

  // Find two random positions
  Eigen::VectorXi boundary_indices(2, 1);
  boundary_indices << firstIndex, secondIndex;

  Eigen::MatrixXd boundaryConditions(2, 2);
  boundaryConditions << 0, 0, 1, 0;

  igl::lscm(mesh->vertices, mesh->faces, boundary_indices, boundaryConditions, result->uvs);

  result->uvs *= scaling;
  result->vertices = mesh->vertices;
  result->faces = mesh->faces;
  result->normals = mesh->normals;

  return result;
}

Configuration LSCM_Parameterizer::getConfiguration() {
  Configuration result;
  result.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Scaling", "Scale the uv's by this amount"}, &scaling, 1, 10});

  result.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"First Index", "Vertex Index for boundary"}, &firstIndex, 0, secondIndex - 1});
  result.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Second Index", "Vertex Index for boundary"}, &secondIndex, 0, indexMax});

  return result;
}

PipelineStageInfo& LSCM_Parameterizer::getInfo() { return info; }
}  // namespace procrock