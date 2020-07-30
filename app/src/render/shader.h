#pragma once

#include <glm/glm.hpp>
#include <map>
#include <string>

namespace procRock {

class Shader {
 public:
  Shader(const std::string vertexPath, const std::string fragmentPath);

  void bind() const;
  void unbind() const;

  std::map<std::string, glm::uvec2> uniforms2ui;
  std::map<std::string, glm::uvec3> uniforms3ui;
  std::map<std::string, glm::uvec4> uniforms4ui;

  std::map<std::string, glm::vec2> uniforms2f;
  std::map<std::string, glm::vec3> uniforms3f;
  std::map<std::string, glm::vec4> uniforms4f;

  std::map<std::string, glm::mat4> uniformsMatrix4f;

  std::map<std::string, float> uniformsf;
  std::map<std::string, int> uniformsi;

 private:
  unsigned int ID;
};
}  // namespace procRock