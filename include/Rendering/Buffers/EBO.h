#pragma once

#include <glad/glad.h>
#include <vector>

class EBO {
public:
  GLuint ID = 0;

  EBO(std::vector<GLuint> &indices) {
    glGenBuffers(1, &ID);
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                 indices.data(), GL_STATIC_DRAW);
  }

  void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
  void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); };
  void Delete() { glDeleteBuffers(1, &ID); }
};
