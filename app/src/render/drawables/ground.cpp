#include "ground.h"

namespace procrock {
DrawableGround::DrawableGround() {
  const int size = 100;
  auto points = std::array<glm::vec3, 4>{glm::vec3{-size, 0, -size}, glm::vec3{-size, 0, size},
                                         glm::vec3{size, 0, -size}, glm::vec3{size, 0, size}};

  for (int i = 0; i < points.size(); i++) {
    this->positions.push_back(points[i]);
    this->colors.push_back(glm::vec3(0.2, 0.2, 0.2));
  }

  this->faces.emplace_back(glm::uvec3{0, 1, 2});
  this->faces.emplace_back(glm::uvec3{1, 3, 2});

  this->texCoords.emplace_back(glm::vec2{0, 0});
  this->texCoords.emplace_back(glm::vec2{0, 50});
  this->texCoords.emplace_back(glm::vec2{50, 0});
  this->texCoords.emplace_back(glm::vec2{50, 50});

  glm::vec3 a = points[1] - points[0];
  glm::vec3 b = points[2] - points[0];
  glm::vec3 normal = glm::normalize(glm::cross(a, b));
  this->normals.push_back(normal);

  a = points[3] - points[1];
  b = points[0] - points[1];
  normal = glm::normalize(glm::cross(a, b));
  this->normals.push_back(normal);

  a = points[0] - points[2];
  b = points[3] - points[2];
  normal = glm::normalize(glm::cross(a, b));
  this->normals.push_back(normal);

  a = points[2] - points[3];
  b = points[1] - points[3];
  normal = glm::normalize(glm::cross(a, b));
  this->normals.push_back(normal);

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
};
}  // namespace procrock