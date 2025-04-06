#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include <vector>

#include "Mesh.h"
#include "Texture.h"

class Model {
public:
  Model(const std::string &path);

  const std::string &GetPath() { return mPath; }
  std::vector<Mesh> &GetMeshes() { return mMeshes; }
  const glm::vec3 &GetMaxVert() const { return mMaxVert; }
  const glm::vec3 &GetMinVert() const { return mMinVert; }
  std::vector<Mesh> &GetFeautureMeshes() { return mFeautureMeshes; }

private:
  void calculateBoundingBox();
  void generateCornerMeshes();

  void loadModel(const std::string &path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<std::string> loadMaterialTextures(aiMaterial *mat,
                                                aiTextureType type);

private:
  std::vector<Mesh> mMeshes;
  std::string mDirectory;
  std::string mPath;
  glm::vec3 mMinVert;
  glm::vec3 mMaxVert;

  std::vector<Mesh> mFeautureMeshes;
};
