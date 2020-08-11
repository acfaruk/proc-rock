#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace procRock {
class Shape {
 public:
  const std::vector<glm::vec3>& getPositions() const;
  const std::vector<glm::vec3>& getNormals() const;
  const std::vector<glm::uvec3>& getFaces() const;
  const std::vector<glm::vec3>& getFaceNormals() const;
  const std::vector<glm::vec3>& getColors() const;
  const std::vector<glm::vec2>& getTexCoords() const;
  const std::vector<glm::uvec2>& getLineIndices() const;

  const glm::mat4& getModelMatrix() const;
  glm::vec3 getPosition();

  void setPosition(glm::vec3);
  void setRotation(float angle, glm::vec3 rot);
  void setScaling(glm::vec3 scale);
  void calculateModelMatrix();

  void addVertex(const glm::vec3& vertex);
  void addFace(const glm::uvec3& face);
  void addNormal(const glm::vec3& normal);
  void addColor(const glm::vec3& color);
  void addTexCoord(const glm::vec3& texCoord);
  void addLine(const glm::uvec2& line);

 protected:
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> colors;
  std::vector<glm::vec3> normals;
  std::vector<glm::uvec3> faces;
  std::vector<glm::vec3> faceNormals;
  std::vector<glm::vec2> texCoords;
  std::vector<glm::vec3> tangents;
  std::vector<glm::uvec2> lineIndices;

  glm::vec3 objectPosition = glm::vec3(0);

  glm::mat4 rotationMatrix = glm::mat4(1);
  glm::mat4 translationMatrix = glm::mat4(1);
  glm::mat4 scalingMatrix = glm::mat4(1);
  glm::mat4 modelMatrix = glm::mat4(1);
};

}  // namespace procRock