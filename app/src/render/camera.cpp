#include "camera.h"

#include <glm/ext.hpp>
#include <iostream>

namespace procRock {
Camera::Camera(glm::uvec2 viewportSize, glm::uvec2 windowSize)
    : Camera(viewportSize, windowSize, glm::pi<float>() / 4.0) {}

Camera::Camera(glm::uvec2 viewportSize, glm::uvec2 windowSize, float fov)
    : viewportSize(viewportSize), windowSize(windowSize), fieldOfView(fov) {}

void Camera::setViewport(glm::uvec2 extent) { this->viewportSize = extent; }
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
  std::cout << "mouse left drag: " << pos1.x << "/" << pos1.y << ".." << pos2.x << "/" << pos2.y
            << std::endl;
}

void Camera::mouseScroll(glm::dvec2 offset) {
  std::cout << "mouse scroll: " << offset.x << "/" << offset.y;
}

void Camera::computeViewProjectionMatrix() {
  this->viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Camera::computeProjectionMatrix() {
  float aspect = float(viewportSize.x) / float(viewportSize.y);
  this->projectionMatrix = glm::perspective(fieldOfView, aspect, nearPlane, farPlane);
}

void Camera::computeViewMatrix() { this->viewMatrix = glm::lookAt(position, target, up); }

}  // namespace procRock