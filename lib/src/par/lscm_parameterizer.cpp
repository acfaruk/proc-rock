#include "par/lscm_parameterizer.h"

#include <igl/adjacency_list.h>
#include <igl/boundary_loop.h>
#include <igl/cut_mesh.h>
#include <igl/lscm.h>
#include <igl/mat_max.h>
#include <igl/mat_min.h>
#include <igl/per_vertex_normals.h>
#include <igl/triangle_triangle_adjacency.h>

#include <array>
#include <set>
#include <stack>

namespace procrock {
std::shared_ptr<Mesh> LSCM_Parameterizer::parameterize(Mesh* mesh) {
  auto result = std::make_shared<Mesh>();

  std::vector<std::vector<int>> adjList;
  igl::adjacency_list(mesh->faces, adjList);

  // Cut the mesh by creating a spanning tree of the vertices

  // Create the spanning tree
  std::stack<int> stack;
  std::vector<bool> visited(mesh->vertices.rows());
  std::vector<std::vector<int>> cuts;

  stack.push(0);
  while (!stack.empty()) {
    int current = stack.top();
    stack.pop();
    visited[current] = true;

    for (int adj : adjList[current]) {
      if (!visited[adj]) {
        visited[adj] = true;
        stack.push(adj);
        std::vector<int> newCuts;
        newCuts.push_back(current);
        newCuts.push_back(adj);
        cuts.push_back(newCuts);
      }
    }
  }

  // Translate cuts of the tree to cuts for igl::cut_mesh

  std::set<std::array<int, 2>> cutEdges;
  for (const auto& cut : cuts) {
    const size_t cutLengh = cut.size();
    for (size_t i = 0; i < cutLengh - 1; i++) {
      std::array<int, 2> e{cut[i], cut[i + 1]};
      if (e[0] > e[1]) {
        std::swap(e[0], e[1]);
      }
      cutEdges.insert(e);
    }
  }

  const size_t numFaces = mesh->faces.rows();
  Eigen::MatrixXi cutMask(numFaces, 3);
  cutMask.setZero();
  for (size_t i = 0; i < numFaces; i++) {
    std::array<int, 2> e0{mesh->faces(i, 0), mesh->faces(i, 1)};
    std::array<int, 2> e1{mesh->faces(i, 1), mesh->faces(i, 2)};
    std::array<int, 2> e2{mesh->faces(i, 2), mesh->faces(i, 0)};
    if (e0[0] > e0[1]) std::swap(e0[0], e0[1]);
    if (e1[0] > e1[1]) std::swap(e1[0], e1[1]);
    if (e2[0] > e2[1]) std::swap(e2[0], e2[1]);

    if (cutEdges.find(e0) != cutEdges.end()) {
      cutMask(i, 0) = 1;
    }
    if (cutEdges.find(e1) != cutEdges.end()) {
      cutMask(i, 1) = 1;
    }
    if (cutEdges.find(e2) != cutEdges.end()) {
      cutMask(i, 2) = 1;
    }
  }

  igl::cut_mesh(mesh->vertices, mesh->faces, cutMask, result->vertices, result->faces);

  // Find the open boundary
  Eigen::VectorXi boundary;
  Eigen::VectorXi boundaryIndices(2, 1);

  igl::boundary_loop(result->faces, boundary);
  boundaryIndices(0) = boundary(0);
  boundaryIndices(1) = boundary(boundary.size() / 2);
  Eigen::MatrixXd boundaryConditions(2, 2);
  boundaryConditions << 0, 0, 1, 0;

  Eigen::MatrixXd uvs;
  igl::lscm(result->vertices, result->faces, boundaryIndices, boundaryConditions, uvs);

  // Normalize uv's to stay between 0 and 1
  Eigen::VectorXd minUVs;
  Eigen::VectorXi indices;
  igl::mat_min(uvs, 1, minUVs, indices);

  uvs = uvs.rowwise() + (-minUVs.transpose());

  Eigen::VectorXd maxUVs;
  igl::mat_max(uvs, 1, maxUVs, indices);
  uvs = uvs.array().rowwise() / (maxUVs.transpose().array());

  result->uvs = uvs * scaling;
  igl::per_vertex_normals(result->vertices, result->faces, result->normals);

  return result;
}

Configuration LSCM_Parameterizer::getConfiguration() {
  Configuration::ConfigurationGroup group;
  group.entry = {"General Settings", "Set various parameters of the displacement."};
  group.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Scaling", "Scale the uv's by this amount"}, &scaling, 0.1, 1});

  Configuration result;
  result.configGroups.push_back(group);
  return result;
}

PipelineStageInfo& LSCM_Parameterizer::getInfo() { return info; }
}  // namespace procrock