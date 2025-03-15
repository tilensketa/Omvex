#include "Shader.h"
#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "Logger.h"

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath) {
  std::string vertexShaderCode = readShaderFile(vertexShaderPath);
  const char *vertexShaderSource = vertexShaderCode.c_str();
  std::string fragmentShaderCode = readShaderFile(fragmentShaderPath);
  const char *fragmentShaderSource = fragmentShaderCode.c_str();

  // Compile Vertex Shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // Check for compilation errors
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::ostringstream errorMsg;
    errorMsg << "Vertex Shader Compilation Failed:\n" << infoLog;
    Logger::getInstance().Error(errorMsg.str());
  }

  // Compile Fragment Shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // Check for compilation errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::ostringstream errorMsg;
    errorMsg << "Fragment Shader Compilation Failed:\n" << infoLog;
    Logger::getInstance().Error(errorMsg.str());
  }

  // Link Shaders into Shader Program
  mID = glCreateProgram();
  glAttachShader(mID, vertexShader);
  glAttachShader(mID, fragmentShader);
  glLinkProgram(mID);

  // Check for linking errors
  glGetProgramiv(mID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(mID, 512, NULL, infoLog);
    std::ostringstream errorMsg;
    errorMsg << "Shader Program Linking Failed:\n" << infoLog;
    Logger::getInstance().Error(errorMsg.str());
  }

  Logger::getInstance().Success(
      "Shader compiled: " + std::string(vertexShaderPath) + ", " +
      std::string(fragmentShaderPath));

  // Delete the shaders as they're linked into our program now and no longer
  // needed
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

std::string Shader::readShaderFile(const char *filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    Logger::getInstance().Error("Failed to open file: " +
                                std::string(filePath));
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  return buffer.str();
}

void Shader::Activate() { glUseProgram(mID); }

void Shader::Delete() { glDeleteProgram(mID); }

void Shader::SetBool(const std::string &name, bool value) const {
  glUniform1i(getLocation(name.c_str()), (int)value);
}
void Shader::SetInt(const std::string &name, int value) const {
  glUniform1i(getLocation(name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, const float value) const {
  glUniform1f(getLocation(name.c_str()), value);
}
void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const {
  glUniform3f(getLocation(name.c_str()), value.x, value.y, value.z);
}
void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const {
  glUniform4f(getLocation(name.c_str()), value.x, value.y, value.z, value.w);
}
void Shader::SetMat4(const std::string &name, const glm::mat4 &value) const {
  glUniformMatrix4fv(getLocation(name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(value));
}

int Shader::getLocation(const std::string &name) const {
  int location = glGetUniformLocation(mID, name.c_str());
  return location;
}
