#pragma once

#include "../io/input_manager.h"

namespace procRock {
class Camera : public InputReceiver {
 public:
  Camera(glm::uvec2 viewportSize);
  Camera(glm::uvec2 viewportSize, float fov);

  void setViewport(glm::uvec2 extent);
  void lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up);
  void setPosition(glm::vec3 pos);

  glm::vec3 getPosition() const;
  glm::vec3 getUpVector() const;
  glm::vec3 getRightVector() const;
  glm::vec3 getTargetVector() const;
  glm::vec3 getForwardVector() const;

  glm::mat4 getViewMatrix() const;
  glm::mat4 getProjectionMatrix() const;
  glm::mat4 getViewProjectionMatrix() const;

  void mouseLeftDrag(glm::dvec2 pos1, glm::dvec2 pos2) override;
  void mouseScroll(glm::dvec2 offset) override;

 private:
  void computeViewProjectionMatrix();
  void computeProjectionMatrix();
  void computeViewMatrix();

  glm::dvec3 screenToSphere(glm::dvec2 screenPos);

  glm::vec3 position = glm::vec3(0);
  glm::vec3 target = glm::vec3(0);
  glm::vec3 up = glm::vec3(0);

  glm::uvec2 viewportSize;

  glm::mat4 viewMatrix = glm::mat4(1);
  glm::mat4 projectionMatrix = glm::mat4(1);
  glm::mat4 viewProjectionMatrix = glm::mat4(1);

  float fieldOfView = 1.0f;
  float nearPlane = 0.01f;
  float farPlane = 100.0f;

  float minDistance = 1.0f;
  float maxDistance = 50.0f;
};
}  // namespace procRock