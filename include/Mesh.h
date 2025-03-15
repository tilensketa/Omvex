#pragma once

#include <string>

#include "Camera.h"
#include "EBO.h"
#include "Texture.h"
#include "VAO.h"

class Mesh {
public:
  Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices,
       std::vector<std::string> &textures, std::string name = "");

  static Mesh CreatePlane(float size, const glm::vec3 &color);
  static Mesh CreateCube(float size, const glm::vec3 &color);
  static Mesh CreateSphere(float radius, const glm::vec3 &color);
  static Mesh CreateCamera();
  static Mesh CreateFrustum(const Camera &cam, float nearPlane, float farPlane,
                            const glm::vec3 &color);

  void Draw(Shader &shader, Camera &camera, GLuint mode) const;

  void SetTransform(const glm::mat4 &mat);

  const std::vector<Vertex> &GetVertices() const { return mVertices; }
  const std::vector<GLuint> &GetIndices() const { return mIndices; }
  // std::vector<Texture> GetTextures() { return mTextures; }
  std::shared_ptr<Texture> GetTexture() { return mTexture; }
  const glm::mat4 &GetMatrix() { return mModel; }
  const std::string &GetName() { return mName; }

private:
  std::vector<Vertex> mVertices;
  std::vector<GLuint> mIndices;
  // std::vector<Texture> mTextures;
  std::shared_ptr<Texture> mTexture = nullptr;
  VAO mVAO;

  glm::mat4 mModel = glm::mat4(1.0f);
  std::string mName;
};
