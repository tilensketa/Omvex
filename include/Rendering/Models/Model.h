#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include <vector>

#include "Rendering/Models/Mesh.h"

class Model {
public:
  Model(const std::string &path, TextureManager *texMng);

  const std::string &GetPath() { return mPath; }
  std::vector<Mesh> &GetMeshes() { return mMeshes; }
  const glm::vec3 &GetMaxVert() const { return mMaxVert; }
  const glm::vec3 &GetMinVert() const { return mMinVert; }
  void Draw(Shader &shader, Camera &camera, const glm::vec3 &color);

  void SetModelMatrix(const glm::mat4 &model) { mModel = model; }
  glm::mat4 GetModelMatrix() const { return mModel; }

private:
  void calculateBoundingBox();

  void loadModel(const std::string &path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<std::string> loadMaterialTextures(aiMaterial *mat,
                                                aiTextureType type);

private:
  glm::mat4 mModel = glm::mat4(1.0f);
  std::vector<Mesh> mMeshes;

  std::string mPath;
  glm::vec3 mMinVert;
  glm::vec3 mMaxVert;

  TextureManager *mTextureManager = nullptr;
};
