#pragma once

#include <glad/glad.h>
#include <memory>

#include "Rendering/Buffers/VAO.h"
#include "Rendering/Textures/Texture.h"

class Quad {
public:
  Quad();
  ~Quad();

  void Draw();
  void SetTexture(Texture *texture);

  Texture *GetTexture() { return mTexture; }

private:
  std::unique_ptr<VAO> mVAO;
  std::unique_ptr<VBO<float>> mVBO;
  Texture *mTexture;
};
