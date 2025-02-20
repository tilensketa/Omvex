#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

#include "Logger.h"

class Texture {
public:
  Texture(const std::string &filePath);
  Texture(int width, int height);
  ~Texture();

  void Bind() const;
  void Unbind() const;

  void Save(const std::string& path);

  glm::vec2 GetSize() const { return mSize; }
  const GLuint GetTextureID() const { return mTextureID; }
  const std::string &GetFilePath() const { return mFilePath; }

private:
  GLuint loadTexture(const std::string &filePath);

private:
  GLuint mTextureID;
  int mWidth, mHeight, mChannels;
  float mAspectRatio;
  glm::vec2 mSize;
  std::string mFilePath;
};
