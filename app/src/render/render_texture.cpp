#include "render_texture.h"

#include "gl_includes.h"

namespace procrock {
RenderTexture::RenderTexture() { glGenTextures(1, &ID); }
RenderTexture::~RenderTexture() { glDeleteTextures(1, &ID); }

void RenderTexture::bind() const {
  assert(ID != 0);
  glBindTexture(GL_TEXTURE_2D, ID);
}
void RenderTexture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
void RenderTexture::loadFromData(unsigned char* data, int width, int height) {
  size.x = width;
  size.y = height;

  glBindTexture(GL_TEXTURE_2D, ID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);
}
const unsigned int RenderTexture::getID() const { return ID; }
}  // namespace procrock