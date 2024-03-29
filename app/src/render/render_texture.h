#pragma once
#include <glm/glm.hpp>
#include <string>

namespace procrock {
class RenderTexture {
 public:
  RenderTexture();
  ~RenderTexture();

  void bind() const;
  void unbind() const;

  void loadFromData(unsigned char* data, int width, int height, int channels = 3);
  void loadFromData(float* data, int width, int height, int channels = 1);
  void loadFromFile(std::string filePath, int channels = 3);

  const unsigned int getID() const;

 private:
  unsigned int ID = 0;
  glm::vec2 size = glm::vec2(0, 0);
};
}  // namespace procrock