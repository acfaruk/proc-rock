#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "camera.h"
#include "shader.h"

namespace procRock {
class Drawable {
 public:
  enum class Primitive {
    POINT = 0x0000,
    LINE = 0x0001,
    TRIANGLE = 0x0004
  } primitiveType = Primitive::TRIANGLE;

  Drawable() = default;
  Drawable(const Drawable&) = delete;
  Drawable(Drawable&&) = delete;
  Drawable& operator=(const Drawable&) = delete;
  Drawable& operator=(Drawable&&) = delete;
  virtual ~Drawable();

  virtual void draw(const Camera& cam, Shader& shader, bool updateMVP = true) const;

  void createBuffers(unsigned int posAttribLoc = 0, unsigned int colAttribLoc = 1,
                     unsigned int normAttribLoc = 2, unsigned int texCoordsAttribLoc = 3);

 protected:
  unsigned int getIndexBufferId() const;
  unsigned int getVertexArrayId() const;
  virtual int getDrawElementsCount() const = 0;

 private:
  unsigned int vaoID = 0;
  unsigned int posBufferID = 0;
  unsigned int colorBufferID = 0;
  unsigned int normalBufferID = 0;
  unsigned int texCoordsBufferID = 0;
  unsigned int indexBufferID = 0;

  virtual const std::vector<glm::vec3>& getPositions() const = 0;
  virtual const std::vector<glm::vec3>& getNormals() const = 0;
  virtual const std::vector<glm::uvec3>& getFaces() const = 0;
  virtual const std::vector<glm::vec3>& getFaceNormals() const = 0;
  virtual const std::vector<glm::vec3>& getColors() const = 0;
  virtual const std::vector<glm::vec2>& getTexCoords() const = 0;
  virtual const glm::mat4& getModelMatrix() const = 0;
  virtual const std::vector<glm::uvec2>& getLineIndices() const = 0;

  virtual void initIndexBuffer();
};
}  // namespace procRock