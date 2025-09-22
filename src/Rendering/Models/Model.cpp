#include "Rendering/Models/Model.h"

#include "Utilities/FileSystem.h"

Model::Model(const std::string &path, TextureManager *texMng)
    : mTextureManager(texMng) {
  loadModel(path);
  Logger::Success("Created model " + path);
}

void Model::loadModel(const std::string &path) {
  Logger::Debug("Loading model: " + path);
  mPath = path;
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    Logger::Error("ERROR::ASSIMP:: " + std::string(importer.GetErrorString()));
    return;
  }

  processNode(scene->mRootNode, scene);
  calculateBoundingBox();
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    mMeshes.push_back(processMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<std::string> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
                                mesh->mVertices[i].z);
    vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
                              mesh->mNormals[i].z);

    if (mesh->mTextureCoords[0]) {
      vertex.TexCoords =
          glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    } else {
      vertex.TexCoords = glm::vec2(0.0f, 0.0f);
    }

    if (mesh->HasVertexColors(0)) {
      vertex.Color = glm::vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g,
                               mesh->mColors[0][i].b);
    } else {
      vertex.Color = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    indices.insert(indices.end(), face.mIndices,
                   face.mIndices + face.mNumIndices);
  }

  if (mesh->mMaterialIndex >= 0 && mTextureManager) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<std::string> diffuseMaps =
        loadMaterialTextures(material, aiTextureType_DIFFUSE);
    if (!diffuseMaps.empty()) textures = diffuseMaps;
  }
  return Mesh(vertices, indices, textures, mTextureManager);
}

std::vector<std::string> Model::loadMaterialTextures(aiMaterial *mat,
                                                     aiTextureType type) {
  std::vector<std::string> textures;
  std::string directory = FileSystem::GetDirectoryFromPath(mPath);
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    std::string texturePath = directory + "/" + str.C_Str();
    textures.push_back(texturePath);
  }
  return textures;
}

void Model::calculateBoundingBox() {
  if (mMeshes.empty()) return;

  mMinVert = mMeshes[0].GetVertices()[0].Position;
  mMaxVert = mMinVert;

  for (const auto &mesh : mMeshes) {
    for (const auto &vertex : mesh.GetVertices()) {
      mMinVert = glm::min(mMinVert, vertex.Position);
      mMaxVert = glm::max(mMaxVert, vertex.Position);
    }
  }
}

void Model::Draw(Shader &shader, Camera &camera, const glm::vec3 &color) {
  shader.Activate();
  shader.SetMat4("uModel", mModel);

  for (const Mesh &mesh : mMeshes) {
    shader.SetVec3("uUniqueColor", color);
    mesh.Draw(shader, camera, true);
  }
}
