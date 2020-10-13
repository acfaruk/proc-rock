#include "point_light.h"

#include <glm/gtx/euler_angles.hpp>

namespace procrock {
void PointLight::setEulerAngles(float yaw, float pitch) {
  glm::mat4 rotMatrix = glm::eulerAngleZY(pitch, yaw);
  position = glm::vec4(initialPosition, 0) * rotMatrix;
}
}  // namespace procrock
