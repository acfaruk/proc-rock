#pragma once

#include <glm/glm.hpp>

namespace procRock {

class PointLight {
 public:
  PointLight(glm::vec3 position, glm::vec3 color = glm::vec3(1, 1, 1))
      : initialPosition(position), position(position), color(color){};

  glm::vec3 getPosition() const;
  glm::vec3 getColor() const;

  void setEulerAngles(float yaw, float pitch);

 private:
  glm::vec3 initialPosition;
  glm::vec3 position;
  glm::vec3 color;
};
}  // namespace procRock