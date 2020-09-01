#include "drawable.h"

#include <numeric>

#include "gl_includes.h"

namespace procrock {
Drawable::~Drawable() {
  if (this->posBufferID != 0) {
    glDeleteBuffers(1, &this->posBufferID);
  }
  if (this->colorBufferID != 0) {
    glDeleteBuffers(1, &this->colorBufferID);
  }
  if (this->normalBufferID != 0) {
    glDeleteBuffers(1, &this->normalBufferID);
  }
  if (this->tangentBufferID != 0) {
    glDeleteBuffers(1, &this->tangentBufferID);
  }
  if (this->texCoordsBufferID != 0) {
    glDeleteBuffers(1, &this->texCoordsBufferID);
  }
  if (this->indexBufferID != 0) {
    glDeleteBuffers(1, &this->indexBufferID);
  }
  if (this->vaoID != 0) {
    glDeleteVertexArrays(1, &this->vaoID);
  }
}
void Drawable::draw(const Camera& cam, Shader& shader, bool updateMVP) const {
  const std::vector<glm::vec3>& colors = getColors();

  if (updateMVP) {
    glm::mat4 mvpMatrix = cam.getViewProjectionMatrix() * this->getModelMatrix();
    shader.uniformsMatrix4f["modelMatrix"] = this->getModelMatrix();
    shader.uniformsMatrix4f["mvpMatrix"] = mvpMatrix;
  }
  shader.uniformsi["vertexColored"] = colors.size() > 0;

  shader.bind();
  glBindVertexArray(getVertexArrayId());
  glDrawElements((int)primitiveType, this->getDrawElementsCount(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  shader.unbind();
}

void Drawable::createBuffers(unsigned int posAttribLoc, unsigned int colAttribLoc,
                             unsigned int normAttribLoc, unsigned int tanAttribLoc,
                             unsigned int texCoordsAttribLoc) {
  const std::vector<glm::vec3>& positions = getPositions();
  const std::vector<glm::vec3>& colors = getColors();
  const std::vector<glm::vec3>& normals = getNormals();
  const std::vector<glm::vec3>& tangents = getTangents();
  const std::vector<glm::vec2>& texCoords = getTexCoords();

  // Initialize Vertex Array Object
  glGenVertexArrays(1, &vaoID);
  glBindVertexArray(vaoID);

  // Initialize buffer objects with geometry data

  // positions
  glGenBuffers(1, &posBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, posBufferID);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(),
               GL_STATIC_DRAW);
  glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(posAttribLoc);

  // colors
  if (colors.size() > 0) {
    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(colAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colAttribLoc);
  }

  // normals
  if (normals.size() != 0) {
    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(normAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normAttribLoc);
  }

  // tangents
  if (tangents.size() != 0) {
    glGenBuffers(1, &tangentBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, tangentBufferID);
    glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), tangents.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(tanAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(tanAttribLoc);
  }

  // texture coordinates
  if (texCoords.size() != 0) {
    glGenBuffers(1, &texCoordsBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordsBufferID);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(texCoordsAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(texCoordsAttribLoc);
  }

  glGenBuffers(1, &indexBufferID);
  this->initIndexBuffer();

  // Reset state
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return;
}

unsigned int Drawable::getIndexBufferId() const { return indexBufferID; }
unsigned int Drawable::getVertexArrayId() const { return vaoID; }
int Drawable::getDrawElementsCount() const {
  switch (primitiveType) {
    case Primitive::POINT:
      return (int)getPositions().size();
      break;
    case Primitive::LINE:
      return (int)getLineIndices().size() * 2;
      break;
    case Primitive::TRIANGLE:
      return 3 * (int)getFaces().size();
      break;
    default:
      return 0;
      break;
  }
}

void Drawable::initIndexBuffer() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getIndexBufferId());
  switch (primitiveType) {
    case Primitive::POINT: {
      size_t size = getPositions().size();
      std::vector<unsigned int> indices;
      indices.reserve(size);
      std::iota(indices.begin(), indices.end(), 0);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), indices.data(),
                   GL_STATIC_DRAW);
      break;
    }
    case Primitive::LINE: {
      const std::vector<glm::uvec2>& lineIndices = getLineIndices();
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(glm::uvec2),
                   lineIndices.data(), GL_STATIC_DRAW);
      break;
    }
    case Primitive::TRIANGLE: {
      const std::vector<glm::uvec3>& faces = getFaces();
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(glm::uvec3), faces.data(),
                   GL_STATIC_DRAW);
      break;
    }
  }
}
}  // namespace procrock