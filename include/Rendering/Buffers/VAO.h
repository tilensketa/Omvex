#pragma once

#include "Rendering/Buffers/VBO.h"
#include <glad/glad.h>

class VAO {
public:
  GLuint ID = 0;

  VAO() { glGenVertexArrays(1, &ID); }

  template <typename T>
  void LinkAttrib(VBO<T> &VBO, GLuint layout, GLuint numComponents, GLenum type,
                  GLsizeiptr stride, void *offset) {
    VBO.Bind();
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE,
                          static_cast<GLsizei>(stride), offset);
    glEnableVertexAttribArray(layout);
    VBO.Unbind();
  }
  void Bind() const { glBindVertexArray(ID); }
  void Unbind() const { glBindVertexArray(0); }
  void Delete() { glDeleteVertexArrays(1, &ID); }
};
