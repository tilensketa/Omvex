#include "Mesh.h"
#include <glm/gtx/euler_angles.hpp>

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices,
           std::vector<std::string> &textures, std::string name) {
  Logger::getInstance().Debug("Creating mesh: " + name);
  mVertices = vertices;
  mIndices = indices;
  // Only use first texture
  for (const std::string &texturePath : textures) {
    mTexture = std::make_shared<Texture>(texturePath);
    break;
  }
  mName = name;

  mVAO.Bind();
  VBO mVBO(vertices);
  EBO mEBO(indices);
  mVAO.LinkAttrib(mVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
  mVAO.LinkAttrib(mVBO, 1, 3, GL_FLOAT, sizeof(Vertex),
                  (void *)(3 * sizeof(float)));
  mVAO.LinkAttrib(mVBO, 2, 3, GL_FLOAT, sizeof(Vertex),
                  (void *)(6 * sizeof(float)));
  mVAO.LinkAttrib(mVBO, 3, 2, GL_FLOAT, sizeof(Vertex),
                  (void *)(9 * sizeof(float)));
  mVAO.Unbind();
  mVBO.Unbind();
  mEBO.Unbind();
}

Mesh Mesh::CreateQuad(float halfSide) {
  std::vector<Vertex> vertices = {{glm::vec3(halfSide, halfSide, 0)},
                                  {glm::vec3(-halfSide, halfSide, 0)},
                                  {glm::vec3(halfSide, -halfSide, 0)},
                                  {glm::vec3(-halfSide, -halfSide, 0)}};

  std::vector<GLuint> indices = {0, 1, 2, 1, 2, 3};
  std::vector<std::string> textures = {};
  return Mesh(vertices, indices, textures, "Quad");
}

Mesh Mesh::CreateCuboid(const glm::vec3 &pos, const glm::vec3 &size,
                        const glm::vec3 &rotation) {
  float halfWidth = size.x * 0.5f;
  float halfHeight = size.y * 0.5f;
  float halfDepth = size.z * 0.5f;

  std::vector<glm::vec3> rawPositions = {
      // Front face
      {-halfWidth, -halfHeight, halfDepth},
      {halfWidth, -halfHeight, halfDepth},
      {halfWidth, halfHeight, halfDepth},
      {-halfWidth, halfHeight, halfDepth},

      // Back face
      {-halfWidth, -halfHeight, -halfDepth},
      {halfWidth, -halfHeight, -halfDepth},
      {halfWidth, halfHeight, -halfDepth},
      {-halfWidth, halfHeight, -halfDepth},
  };

  // Create rotation matrix from Euler angles (XYZ order)
  glm::mat4 rotMatrix =
      glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y),
                         glm::radians(rotation.z));

  std::vector<Vertex> vertices;
  for (const auto &localPos : rawPositions) {
    glm::vec4 rotated = rotMatrix * glm::vec4(localPos, 1.0f);
    vertices.push_back(
        {pos + glm::vec3(rotated)}); // Translate to world position
  }

  std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1,
                                 5, 4, 7, 7, 6, 5, 4, 0, 3, 3, 7, 4,
                                 3, 2, 6, 6, 7, 3, 4, 5, 1, 1, 0, 4};

  std::vector<std::string> textures = {};
  return Mesh(vertices, indices, textures, "Cube");
}

void Mesh::Draw(Shader &shader, Camera &camera, bool fill) const {
  shader.Activate();
  mVAO.Bind();

  bool hasTexture = mTexture != nullptr;
  shader.SetBool("HasTexture", hasTexture);

  if (hasTexture) {
    mTexture->Bind();
    shader.SetInt("tex1", 0);
  }

  shader.SetMat4("model", mModel);
  shader.SetVec3("camPos", camera.GetPosition());
  camera.Matrix(shader, "camMatrix");

  if (fill)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawElements(GL_TRIANGLES, static_cast<int>(mIndices.size()),
                 GL_UNSIGNED_INT, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  mVAO.Unbind();
}

void Mesh::SetTransform(const glm::mat4 &mat) { mModel = mat; }
