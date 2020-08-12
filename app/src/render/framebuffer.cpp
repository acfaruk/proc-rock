#include "framebuffer.h"

#include <assert.h>

#include <iostream>

namespace procrock {
void bindDefaultFrameBuffer(glm::uvec2 size) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, size.x, size.y);
}

Framebuffer::Framebuffer(const glm::uvec2& size, unsigned int textureCount) {
  assert(textureCount <= GL_MAX_COLOR_ATTACHMENTS && "To many textures for framebuffer");

  this->size = size;
  glGenFramebuffers(1, &ID);
  glBindFramebuffer(GL_FRAMEBUFFER, ID);

  // Create textures
  for (unsigned int i = 0; i < textureCount; i++) {
    unsigned int renderedTexture;
    glGenTextures(1, &renderedTexture);

    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    renderedTextures.push_back(renderedTexture);
  }

  // Assign textures to framebuffer
  std::vector<unsigned int> drawBuffers;
  for (unsigned int i = 0; i < textureCount; i++) {
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, renderedTextures[i], 0);
    drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
  }
  glDrawBuffers(textureCount, drawBuffers.data());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
  glDeleteTextures((GLsizei)renderedTextures.size(), renderedTextures.data());
  glDeleteFramebuffers(1, &ID);
}

void Framebuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, ID);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, size.x, size.y);
}

void Framebuffer::resize(const glm::uvec2& size) {
  this->size = size;
  for (int i = 0; i < renderedTextures.size(); i++) {
    glBindTexture(GL_TEXTURE_2D, renderedTextures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
  }
}

std::vector<unsigned int>& Framebuffer::getRenderedTextures() { return renderedTextures; }

}  // namespace procrock
