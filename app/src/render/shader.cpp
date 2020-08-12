#include "shader.h"

#include <GL/glew.h>

#include <fstream>
#include <iostream>
#include <sstream>

namespace procrock {
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

void Shader::bind() const {
  glUseProgram(ID);
  for (auto& u : uniformsi) {
    glUniform1i(glGetUniformLocation(ID, u.first.c_str()), u.second);
  }

  for (auto& u : uniformsf) {
    glUniform1f(glGetUniformLocation(ID, u.first.c_str()), u.second);
  }

  for (auto& u : uniforms2ui) {
    glUniform2uiv(glGetUniformLocation(ID, u.first.c_str()), 1, &u.second[0]);
  }

  for (auto& u : uniforms3ui) {
    glUniform3uiv(glGetUniformLocation(ID, u.first.c_str()), 1, &u.second[0]);
  }

  for (auto& u : uniforms4ui) {
    glUniform4uiv(glGetUniformLocation(ID, u.first.c_str()), 1, &u.second[0]);
  }

  for (auto& u : uniforms2f) {
    glUniform2fv(glGetUniformLocation(ID, u.first.c_str()), 1, &u.second[0]);
  }

  for (auto& u : uniforms3f) {
    glUniform3fv(glGetUniformLocation(ID, u.first.c_str()), 1, &u.second[0]);
  }

  for (auto& u : uniforms4f) {
    glUniform4fv(glGetUniformLocation(ID, u.first.c_str()), 1, &u.second[0]);
  }

  for (auto& u : uniformsMatrix4f) {
    glUniformMatrix4fv(glGetUniformLocation(ID, u.first.c_str()), 1, GL_FALSE, &u.second[0][0]);
  }
}

void Shader::unbind() const { glUseProgram(0); }

}  // namespace procrock