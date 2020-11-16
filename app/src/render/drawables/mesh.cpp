#include "mesh.h"

namespace procrock {
DrawableMesh::DrawableMesh(Mesh& mesh) {
  for (Eigen::Index i = 0; i < mesh.vertices.rows(); ++i) {
    auto v_row = mesh.vertices.row(i);
    auto n_row = mesh.normals.row(i);
    auto uv_row = mesh.uvs.row(i);
    this->positions.emplace_back(v_row(0), v_row(1), v_row(2));
    this->normals.emplace_back(n_row(0), n_row(1), n_row(2));
    this->colors.emplace_back(1, 1, 1);
    this->texCoords.emplace_back(uv_row(0), uv_row(1));
  }

  tangents.resize(positions.size());
  for (Eigen::Index i = 0; i < mesh.faces.rows(); ++i) {
    auto f_row = mesh.faces.row(i);
    auto t_row = mesh.faceTangents.row(i);
    this->faces.emplace_back(f_row(0), f_row(1), f_row(2));

    this->tangents[mesh.faces.row(i).x()] = glm::vec3(t_row(0), t_row(1), t_row(2));
    this->tangents[mesh.faces.row(i).y()] = glm::vec3(t_row(0), t_row(1), t_row(2));
    this->tangents[mesh.faces.row(i).z()] = glm::vec3(t_row(0), t_row(1), t_row(2));
  }
  createBuffers();
}
}  // namespace procrock
