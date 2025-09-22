#include "Rendering/Models/Mesh.h"

#include <glm/gtx/euler_angles.hpp>

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices,
           std::vector<std::string> &textures, TextureManager *texMng)
    : mVertices(vertices), mIndices(indices) {
  // Only use first texture // WARN
  if (!textures.empty()) {
    mTexture = texMng->GetTexture(textures[0]);
  }
  setupMesh();
}

void Mesh::setupMesh() {
  mVAO.Bind();
  mVBO = std::make_unique<VBO<Vertex>>(mVertices);
  mEBO = std::make_unique<EBO>(mIndices);

  mVAO.LinkAttrib(*mVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
  mVAO.LinkAttrib(*mVBO, 1, 3, GL_FLOAT, sizeof(Vertex),
                  (void *)(3 * sizeof(float)));
  mVAO.LinkAttrib(*mVBO, 2, 3, GL_FLOAT, sizeof(Vertex),
                  (void *)(6 * sizeof(float)));
  mVAO.LinkAttrib(*mVBO, 3, 2, GL_FLOAT, sizeof(Vertex),
                  (void *)(9 * sizeof(float)));
  mVAO.Unbind();
}

void Mesh::Draw(Shader &shader, Camera &camera, bool fill) const {
  shader.Activate();
  mVAO.Bind();

  shader.SetBool("uHasTexture", mTexture != nullptr);

  if (mTexture) {
    mTexture->Bind();
    shader.SetInt("uTex1", 0);
  }
  shader.SetVec3("uCamPos", camera.GetPosition());
  shader.SetMat4("uCamMatrix", camera.GetMatrix());

  glPolygonMode(GL_FRONT_AND_BACK, fill ? GL_FILL : GL_LINE);
  glDrawElements(GL_TRIANGLES, static_cast<int>(mIndices.size()),
                 GL_UNSIGNED_INT, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  mVAO.Unbind();
}
