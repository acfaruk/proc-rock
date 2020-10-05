#include "render_texture.h"

#include "../io/stb_image.h"
#include "gl_includes.h"

namespace procrock {
RenderTexture::RenderTexture() { glGenTextures(1, &ID); }
RenderTexture::~RenderTexture() { glDeleteTextures(1, &ID); }

void RenderTexture::bind() const {
  assert(ID != 0);
  glBindTexture(GL_TEXTURE_2D, ID);
}
void RenderTexture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
void RenderTexture::loadFromData(unsigned char* data, int width, int height, int channels) {
  size.x = width;
  size.y = height;

  glBindTexture(GL_TEXTURE_2D, ID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  switch (channels) {
    case 1:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, data);
      break;
    case 2:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, size.x, size.y, 0, GL_RG, GL_UNSIGNED_BYTE, data);
      break;
    case 3:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      break;
    case 4:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      break;
    default:
      assert(true && "Only 1 - 4 channels supported.");
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::loadFromFile(std::string filePath, int channels) {
  int x;
  int y;
  int c;
  unsigned char* image = stbi_load(filePath.c_str(), &x, &y, &c, channels);

  size.x = x;
  size.y = y;

  glBindTexture(GL_TEXTURE_2D, ID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  switch (c) {
    case 1:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, image);
      break;
    case 2:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, size.x, size.y, 0, GL_RG, GL_UNSIGNED_BYTE, image);
      break;
    case 3:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
      break;
    case 4:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
      break;
    default:
      assert(true && "Only 1 - 4 channels supported.");
  }
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}

const unsigned int RenderTexture::getID() const { return ID; }
}  // namespace procrock