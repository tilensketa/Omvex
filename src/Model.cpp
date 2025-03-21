#include "Model.h"
#include "Utils.h"

Model::Model(const std::string &path) {
  loadModel(path);
  Logger::getInstance().Success("Created model " + path);
}

void Model::Draw(Shader &shader, Camera &camera, GLuint mode) const {
  for (const auto &mesh : mMeshes) {
    mesh.Draw(shader, camera, mode);
  }
}

void Model::loadModel(const std::string &path) {
  Logger::getInstance().Debug("Loading model: " + path);
  mPath = path;
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    Logger::getInstance().Error("ERROR::ASSIMP:: " +
                                std::string(importer.GetErrorString()));
    return;
  }

  mDirectory = path.substr(0, path.find_last_of('/'));
  processNode(scene->mRootNode, scene);
  calculateBoundingBox();

  Logger::getInstance().Debug("MODEL");
  Logger::getInstance().Debug("|-Model path: " + path);
  Logger::getInstance().Debug("  |- Max: " +
                              Utils::GlmToString::Vec3(mMaxVert));
  Logger::getInstance().Debug("  |- Min: " +
                              Utils::GlmToString::Vec3(mMinVert));
  Logger::getInstance().Debug("|-Number of meshes: " +
                              std::to_string(mMeshes.size()));
  int i = 0;
  for (Mesh &mesh : mMeshes) {
    Logger::getInstance().Debug(" |-Mesh: " + std::to_string(i));
    Logger::getInstance().Debug("  |- Vertices: " +
                                std::to_string(mesh.GetVertices().size()));
    Logger::getInstance().Debug("  |- Indices: " +
                                std::to_string(mesh.GetIndices().size()));

    std::shared_ptr<Texture> texture = mesh.GetTexture();
    if (texture != nullptr) {
      Logger::getInstance().Debug("  |- Texture id: " +
                                  std::to_string(texture->GetTextureID()));
      Logger::getInstance().Debug("   |- Name: " + texture->GetFilePath());
      Logger::getInstance().Debug("   |- SizeX: " +
                                  std::to_string(texture->GetSize().x));
      Logger::getInstance().Debug("   |- SizeY: " +
                                  std::to_string(texture->GetSize().y));
    }
    i++;
  }
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
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<std::string> diffuseMaps =
        loadMaterialTextures(material, aiTextureType_DIFFUSE);
    // textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    textures = diffuseMaps;
  }

  return Mesh(vertices, indices, textures, mesh->mName.C_Str());
}

std::vector<std::string> Model::loadMaterialTextures(aiMaterial *mat,
                                                     aiTextureType type) {
  std::vector<std::string> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    std::string texturePath = mDirectory + "/" + str.C_Str();
    textures.push_back(texturePath);
  }
  return textures;
}

void Model::calculateBoundingBox() {
  Logger::getInstance().Debug("Calculating bounding box");
  for (const Mesh &mesh : mMeshes) {
    const std::vector<Vertex> &vertices = mesh.GetVertices();
    mMinVert = vertices[0].Position;
    mMaxVert = vertices[0].Position;
    for (size_t i = 1; i < vertices.size(); i++) {
      const glm::vec3 pos = vertices[i].Position;
      for (size_t j = 0; j < 3; j++) {
        if (mMinVert[j] > pos[j])
          mMinVert[j] = pos[j];
        if (mMaxVert[j] < pos[j])
          mMaxVert[j] = pos[j];
      }
    }
  }
}
