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

  // Calculate tangents
  tangents.resize(positions.size());
  for (int i = 0; i < faces.size(); i++) {
    glm::vec3& v0 = positions[faces[i].x];
    glm::vec3& v1 = positions[faces[i].y];
    glm::vec3& v2 = positions[faces[i].z];

    glm::vec2& uv0 = texCoords[faces[i].x];
    glm::vec2& uv1 = texCoords[faces[i].y];
    glm::vec2& uv2 = texCoords[faces[i].z];

    glm::vec3 deltaPos1 = v1 - v0;
    glm::vec3 deltaPos2 = v2 - v0;

    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;

    tangents[faces[i].x] = tangent;
    tangents[faces[i].y] = tangent;
    tangents[faces[i].z] = tangent;
  }

  createBuffers();
}
}  // namespace procrock
