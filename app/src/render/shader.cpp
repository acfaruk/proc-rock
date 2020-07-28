#include "shader.h"

#include <GL/glew.h>

#include <fstream>
#include <iostream>
#include <sstream>

namespace procRock {
Shader::Shader(const std::string vertexPath, const std::string fragmentPath) {
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  std::string vertexCode;
  std::string fragmentCode;

  // throw if error happens
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // open files
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure e) {
    std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }

  const char* vShaderCode = vertexCode.c_str();
  const char* fShaderCode = fragmentCode.c_str();

  // 2. compile shaders
  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  // vertex Shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  // print compile errors if any
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  };

  // fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  // print compile errors if any
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  };

  // shader Program
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  // print linking errors if any
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  // delete the shaders as they're linked into our program now and no longer necessery
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::bind() const { glUseProgram(ID); }

void Shader::unbind() const { glUseProgram(0); }

void Shader::setUVec2(const std::string& name, glm::uvec2 value) const {
  glUniform2ui(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}

void Shader::setUVec3(const std::string& name, glm::uvec3 value) const {
  glUniform3ui(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setUVec4(const std::string& name, glm::uvec4 value) const {
  glUniform4ui(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setDVec2(const std::string& name, glm::dvec2 value) const {
  glUniform2d(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}

void Shader::setDVec3(const std::string& name, glm::dvec3 value) const {
  glUniform3d(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setDVec4(const std::string& name, glm::dvec4 value) const {
  glUniform4d(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setFVec2(const std::string& name, glm::vec2 value) const {
  glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}

void Shader::setFVec3(const std::string& name, glm::vec3 value) const {
  glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setFVec4(const std::string& name, glm::vec4 value) const {
  glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setFMat4(const std::string& name, glm::mat4 value) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, (float*)&value);
}

void Shader::setFloat(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setDouble(const std::string& name, double value) const {
  glUniform1d(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

}  // namespace procRock