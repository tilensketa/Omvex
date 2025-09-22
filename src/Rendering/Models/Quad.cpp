#include "Rendering/Models/Quad.h"

Quad::Quad() {
  std::vector<float> quadVertices = {-1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                                     0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

                                     -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                                     1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};
  mVAO = std::make_unique<VAO>();
  mVAO->Bind();

  mVBO = std::make_unique<VBO<float>>(quadVertices);
  mVAO->LinkAttrib(*mVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), (void *)0);
  mVAO->LinkAttrib(*mVBO, 1, 2, GL_FLOAT, 4 * sizeof(float),
                   (void *)(2 * sizeof(float)));

  mVBO->Unbind();
  mVAO->Unbind();
}

Quad::~Quad() {
  mVAO->Delete();
  mVBO->Delete();
}

void Quad::SetTexture(Texture *texture) { mTexture = texture; }

void Quad::Draw() {
  mVAO->Bind();
  glDrawArrays(GL_TRIANGLES, 0, 6);
  mVAO->Unbind();
}
