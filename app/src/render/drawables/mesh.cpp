#include "mesh.h"

namespace procrock {
DrawableMesh::DrawableMesh(Mesh& mesh) {
  for (Eigen::Index i = 0; i < mesh.vertices.rows(); ++i) {
    auto v_row = mesh.vertices.row(i);
    auto n_row = mesh.normals.row(i);
    this->positions.emplace_back(v_row(0), v_row(1), v_row(2));
    this->normals.emplace_back(n_row(0), n_row(1), n_row(2));
    this->colors.emplace_back(1, 1, 1);
  }

  for (Eigen::Index i = 0; i < mesh.faces.rows(); ++i) {
    auto row = mesh.faces.row(i);
    this->faces.emplace_back(row(0), row(1), row(2));
  }

  for (Eigen::Index i = 0; i < mesh.uvs.rows(); ++i) {
    auto row = mesh.uvs.row(i);
    this->texCoords.emplace_back(row(0), row(1));
  }

  createBuffers();
}
}  // namespace procrock
