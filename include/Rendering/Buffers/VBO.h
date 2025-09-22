#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec3 Color;
  glm::vec2 TexCoords;
};

template <typename T> class VBO {
public:
  GLuint ID = 0;

  VBO(std::vector<T> &data) {
    glGenBuffers(1, &ID);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(),
                 GL_STATIC_DRAW);
    Unbind();
  }

  void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, ID); }
  void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
  void Delete() { glDeleteBuffers(1, &ID); }
};
