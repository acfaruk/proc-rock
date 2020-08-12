#include "camera.h"

#include <glm/ext.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

namespace procrock {
Camera::Camera(glm::uvec2 viewportSize) : Camera(viewportSize, glm::pi<float>() / 4.0f) {}

Camera::Camera(glm::uvec2 viewportSize, float fov)
    : InputReceiver(true), viewportSize(viewportSize), fieldOfView(fov) {}

void Camera::setViewport(glm::uvec2 extent) {
  this->viewportSize = extent;
  computeProjectionMatrix();
  computeViewProjectionMatrix();
}
void Camera::lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up) {
  this->position = pos;
  this->target = target;
  glm::vec3 v = target - position;
  glm::vec3 right = glm::cross(v, up);
  this->up = glm::normalize(glm::cross(right, v));

  computeViewMatrix();
  computeProjectionMatrix();
  computeViewProjectionMatrix();
}

void Camera::setPosition(glm::vec3 pos) { this->position = pos; }

glm::vec3 Camera::getPosition() const { return this->position; }

glm::vec3 Camera::getUpVector() const { return this->up; }

glm::vec3 Camera::getRightVector() const {
  return glm::normalize(glm::cross(getForwardVector(), up));
}

glm::vec3 Camera::getTargetVector() const { return this->target; }

glm::vec3 Camera::getForwardVector() const { return glm::normalize(target - position); }

glm::mat4 Camera::getViewMatrix() const { return this->viewMatrix; }

glm::mat4 Camera::getProjectionMatrix() const { return this->projectionMatrix; }

glm::mat4 Camera::getViewProjectionMatrix() const { return this->viewProjectionMatrix; }

void Camera::mouseLeftDrag(glm::dvec2 pos1, glm::dvec2 pos2) {
  if (glm::distance2(pos1, pos2) <= 0.01) return;
  glm::dvec2 delta = pos2 - pos1;
  glm::dvec3 spherePos1 = screenToSphere(pos1);
  glm::dvec3 spherePos2 = screenToSphere(glm::vec2(pos2.x - 2 * (delta.x), pos2.y));

  glm::vec3 rotAxis = glm::cross(spherePos1, spherePos2);
  float angle = (float)glm::acos(glm::min(1.0, glm::dot(spherePos1, spherePos2)));

  glm::mat4 rotMatrix = glm::rotate(glm::mat4(1), angle, rotAxis);

  glm::vec4 targetToCamera = glm::vec4(position - target, 0);
  glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);

  targetToCamera = inverseViewMatrix * rotMatrix * viewMatrix * targetToCamera;

  setPosition(target + glm::vec3(targetToCamera));

  up = glm::vec3(0, 1, 0);

  float angleUp = glm::acos(glm::min(1.0f, glm::dot(getForwardVector(), up)));
  if (angleUp < 0.1 || angleUp > glm::pi<float>() - 0.1) {
    return;
  }
  computeViewMatrix();
  computeProjectionMatrix();
}

void Camera::mouseScroll(glm::dvec2 offset) {
  float distanceSquared = glm::distance2(position, target);

  glm::vec3 v = position - target;
  glm::vec3 newV = (1.0f + (float)(-offset.y / sqrt(distanceSquared))) * v;

  if (glm::length2(newV) <= minDistance) return;
  if (glm::length2(newV) >= maxDistance) return;
  position = target + newV;

  computeViewMatrix();
  computeViewProjectionMatrix();
}

void Camera::computeViewProjectionMatrix() {
  this->viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Camera::computeProjectionMatrix() {
  float aspect = float(viewportSize.x) / float(viewportSize.y);
  this->projectionMatrix = glm::perspective(fieldOfView, aspect, nearPlane, farPlane);
}

void Camera::computeViewMatrix() { this->viewMatrix = glm::lookAt(position, target, up); }

glm::dvec3 Camera::screenToSphere(glm::dvec2 screenPos) {
  glm::dvec2 center = glm::dvec2(viewportSize.x / 2.0, viewportSize.y / 2.0);
  double radius = viewportSize.x > viewportSize.y ? viewportSize.y / 2.0 : viewportSize.x / 2.0;

  glm::dvec3 result;

  double radiusSquared = pow(radius, 2);

  double screenMinusCenterX = screenPos.x - center.x;
  double screenMinusCenterY = screenPos.y - center.y;

  if (glm::distance2(center, screenPos) > radiusSquared) {
    // P(x,y) is outside of sphere
    double firstAddend = pow(screenMinusCenterX, 2) / radiusSquared;
    double secondAddend = pow(screenMinusCenterY, 2) / radiusSquared;

    double s = 1 / sqrt(firstAddend + secondAddend);

    result.x = s * screenMinusCenterX / radius;
    result.y = s * screenMinusCenterY / radius;
    result.z = 0;
  } else {
    // P(x,y) is inside of sphere
    result.x = screenMinusCenterX / radius;
    result.y = screenMinusCenterY / radius;
    result.z = sqrt(1 - pow(result.x, 2) - pow(result.y, 2));
  }
  return result;
}

}  // namespace procrock