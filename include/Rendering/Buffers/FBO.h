#pragma once

#include <glad/glad.h>

#include "Rendering/Textures/Texture.h"

#include <memory>

class FBO {
public:
  GLuint ID = 0;
  std::unique_ptr<Texture> ColorTexture;
  GLuint DepthStencilID = 0;

  FBO(int width, int height);

  void Bind() const;
  void Unbind() const;

  void BindRead() const;
  void BindDraw() const;
  void Delete();

  void Resize(int newWidth, int newHeight);

private:
  void recreateFramebuffer(int width, int height);
  void createColorAttachment(int width, int height);
  void createDepthStencilAttachment(int width, int height);
  void checkComplete() const;
};
