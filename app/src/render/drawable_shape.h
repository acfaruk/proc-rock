#pragma once

#include "drawable.h"
#include "shape.h"

namespace procRock {
class DrawableShape : public Drawable, public Shape {
 public:
  const std::vector<glm::vec3>& getPositions() const override;
  const std::vector<glm::vec3>& getNormals() const override;
  const std::vector<glm::uvec3>& getFaces() const override;
  const std::vector<glm::vec3>& getFaceNormals() const override;
  const std::vector<glm::vec3>& getColors() const override;
  const std::vector<glm::vec2>& getTexCoords() const override;
  const glm::mat4& getModelMatrix() const override;
  virtual int getDrawElementsCount() const override;
  const std::vector<glm::uvec2>& getLineIndices() const override;
};
}  // namespace procRock