#pragma once

#include <glm/glm.hpp>
#include <string>

namespace procRock {

class Shader {
 public:
  Shader(const std::string vertexPath, const std::string fragmentPath);

  void bind() const;
  void unbind() const;

  void setUVec2(const std::string& name, glm::uvec2 value) const;
  void setUVec3(const std::string& name, glm::uvec3 value) const;
  void setUVec4(const std::string& name, glm::uvec4 value) const;

  void setDVec2(const std::string& name, glm::dvec2 value) const;
  void setDVec3(const std::string& name, glm::dvec3 value) const;
  void setDVec4(const std::string& name, glm::dvec4 value) const;

  void setFVec2(const std::string& name, glm::vec2 value) const;
  void setFVec3(const std::string& name, glm::vec3 value) const;
  void setFVec4(const std::string& name, glm::vec4 value) const;

  void setFMat4(const std::string& name, glm::mat4 value) const;

  void setFloat(const std::string& name, float value) const;
  void setDouble(const std::string& name, double value) const;
  void setInt(const std::string& name, int value) const;

  unsigned int ID;
};
}  // namespace procRock