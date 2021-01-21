#pragma once

#include <glm/glm.hpp>

namespace procrock {

class PointLight {
 public:
  PointLight(glm::vec3 position, float intensity, glm::vec3 color = glm::vec3(1, 1, 1))
      : initialPosition(position), position(position), color(color), intensity(intensity){};

  glm::vec3 position;
  glm::vec3 color;
  float intensity = 10;

  void setEulerAngles(float yaw, float pitch);

 private:
  glm::vec3 initialPosition;
};
}  // namespace procrock