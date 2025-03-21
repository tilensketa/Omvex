#pragma once

#include <glad/glad.h>

#include "Texture.h"

#include <memory>

class FBO {
public:
  GLuint ID;
  std::unique_ptr<Texture> colorTexture; // Color attachment texture
  GLuint depthStencilBuffer;             // Depth-stencil renderbuffer

  FBO(int width, int height);

  void Bind();
  void Unbind();
  void Delete();

  void Resize(int newWidth, int newHeight);
};
