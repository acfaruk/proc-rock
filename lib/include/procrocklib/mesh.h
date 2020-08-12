#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace procrock {

struct Mesh {
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec3> colors;
  std::vector<glm::uvec3> faces;
  std::vector<glm::vec2> texCoords;

  void Test();
};
}  // namespace procrock