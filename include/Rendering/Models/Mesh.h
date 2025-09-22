#pragma once

#include "Core/Camera/Camera.h"
#include "Managers/TextureManager.h"
#include "Rendering/Buffers/EBO.h"
#include "Rendering/Buffers/VAO.h"
#include "Rendering/Shaders/Shader.h"
#include "Rendering/Textures/Texture.h"

class Mesh {
public:
  Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices,
       std::vector<std::string> &textures, TextureManager *texMng);

  void Draw(Shader &shader, Camera &camera, bool fill) const;

  const std::vector<Vertex> &GetVertices() const { return mVertices; }
  const std::vector<GLuint> &GetIndices() const { return mIndices; }
  Texture *GetTexture() { return mTexture; }

private:
  void setupMesh();

private:
  std::vector<Vertex> mVertices;
  std::vector<GLuint> mIndices;
  Texture *mTexture = nullptr;

  VAO mVAO;
  std::unique_ptr<VBO<Vertex>> mVBO;
  std::unique_ptr<EBO> mEBO;
};
