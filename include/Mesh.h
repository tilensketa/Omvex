#pragma once

#include <string>

#include "Camera.h"
#include "EBO.h"
#include "VAO.h"

class Mesh {
public:
  Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices,
       std::string name = "");

  static Mesh CreatePlane(float size, const glm::vec3 &color);
  static Mesh CreateCube(float size, const glm::vec3 &color);
  static Mesh CreateSphere(float radius, const glm::vec3 &color);
  static Mesh CreateCamera();
  static Mesh CreateFrustum(const Camera &cam, float nearPlane, float farPlane,
                            const glm::vec3 &color);

  void Draw(Shader &shader, Camera &camera, GLuint mode) const;

  void SetTransform(const glm::mat4 &mat);

  std::vector<Vertex> GetVertices() { return mVertices; }
  std::vector<GLuint> GetIndices() { return mIndices; }
  const glm::mat4 &GetMatrix() { return mModel; }
  const std::string &GetName() { return mName; }

private:
  std::vector<Vertex> mVertices;
  std::vector<GLuint> mIndices;
  VAO mVAO;

  glm::mat4 mModel = glm::mat4(1.0f);
  std::string mName;
};
