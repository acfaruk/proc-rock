#include "par/lscm_parameterizer.h"

#include <igl/boundary_loop.h>
#include <igl/cut_mesh.h>
#include <igl/lscm.h>
#include <igl/per_vertex_normals.h>
#include <igl/triangle_triangle_adjacency.h>

#include <array>

namespace procrock {
std::shared_ptr<Mesh> LSCM_Parameterizer::parameterize(Mesh* mesh) {
  auto result = std::make_shared<Mesh>();
  indexMax = mesh->vertices.rows() - 1;

  Eigen::MatrixXi toCut(mesh->faces.rows(), 3);
  toCut.fill(0);

  Eigen::MatrixXi tta;
  igl::triangle_triangle_adjacency(mesh->faces, tta);

  int adj = tta(0, 0);
  int adj2 = tta(0, 1);
  int other;
  int other2;

  for (int i = 0; i < 3; i++) {
    if (tta(adj, i) == 0) {
      other = i;
    }
    if (tta(adj2, i) == 0) {
      other2 = i;
    }
  }

  toCut(0, 0) = true;
  toCut(0, 1) = true;
  toCut(adj, other) = true;
  toCut(adj2, other2) = true;

  igl::cut_mesh(mesh->vertices, mesh->faces, toCut, result->vertices, result->faces);

  // Find the open boundary
  Eigen::VectorXi boundary;
  Eigen::VectorXi boundaryIndices(2, 1);

  igl::boundary_loop(result->faces, boundary);
  boundaryIndices(0) = boundary(0);
  boundaryIndices(1) = boundary(boundary.size() / 2);
  Eigen::MatrixXd boundaryConditions(2, 2);
  boundaryConditions << 0, 0, 1, 0;

  igl::lscm(result->vertices, result->faces, boundaryIndices, boundaryConditions, result->uvs);
  result->uvs *= scaling;
  igl::per_vertex_normals(result->vertices, result->faces, result->normals);

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