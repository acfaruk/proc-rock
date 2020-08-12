#include "shape.h"

#include <glm/gtc/matrix_transform.hpp>

namespace procrock {

const std::vector<glm::vec3> &Shape::getPositions() const { return this->positions; }
const std::vector<glm::vec3> &Shape::getNormals() const { return this->normals; }
const std::vector<glm::uvec3> &Shape::getFaces() const { return this->faces; }
const std::vector<glm::vec3> &Shape::getFaceNormals() const { return this->faceNormals; }
const std::vector<glm::vec3> &Shape::getColors() const { return this->colors; }
const std::vector<glm::vec2> &Shape::getTexCoords() const { return this->texCoords; }
const std::vector<glm::uvec2> &Shape::getLineIndices() const { return lineIndices; }

const glm::mat4 &Shape::getModelMatrix() const { return this->modelMatrix; }
glm::vec3 Shape::getPosition() { return this->objectPosition; }

void Shape::setPosition(glm::vec3 pos) {
  this->objectPosition = pos;
  this->translationMatrix = glm::translate(glm::mat4(1), pos);
  this->calculateModelMatrix();
}

void Shape::setRotation(float angle, glm::vec3 rot) {
  this->rotationMatrix = glm::rotate(this->rotationMatrix, glm::radians(angle), rot);
  this->calculateModelMatrix();
}

void Shape::setScaling(glm::vec3 scale) {
  this->scalingMatrix = glm::scale(glm::mat4(1), scale);
  this->calculateModelMatrix();
}

void Shape::calculateModelMatrix() {
  this->modelMatrix = translationMatrix * rotationMatrix * scalingMatrix * glm::mat4(1);
}

void Shape::addVertex(const glm::vec3 &vertex) { this->positions.push_back(vertex); }
void Shape::addFace(const glm::uvec3 &face) { faces.push_back(face); }
void Shape::addNormal(const glm::vec3 &norm) { normals.push_back(norm); }
void Shape::addColor(const glm::vec3 &color) { colors.push_back(color); }
void Shape::addTexCoord(const glm::vec3 &texCoord) { texCoords.push_back(texCoord); }
void Shape::addLine(const glm::uvec2 &line) { lineIndices.push_back(line); }

}  // namespace procrock