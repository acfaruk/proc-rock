#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "gl_includes.h"

namespace procrock {

void bindDefaultFrameBuffer(glm::uvec2 size);

class Framebuffer {
 public:
  Framebuffer(const glm::uvec2 &size, unsigned int textureCount = 1);
  ~Framebuffer();
  void bind();
  void resize(const glm::uvec2 &size);
  std::vector<unsigned int> &getRenderedTextures();

 private:
  unsigned int ID;
  std::vector<unsigned int> renderedTextures;
  glm::uvec2 size;
};
}  // namespace procrock