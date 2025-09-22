#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Texture {
public:
  Texture(const std::string &filePath);
  Texture(int width, int height, int channels=4);
  Texture(const std::vector<glm::vec3> &pixelColors);
  ~Texture();

  void Bind() const;
  void Unbind() const;

  void Save(const std::string &path);

  glm::vec2 GetSize() const { return mSize; }
  const GLuint GetTextureID() const { return mTextureID; }
  const std::string &GetFilePath() const { return mFilePath; }

private:
  void initializeCommonMembers(int width, int height, int channels);
  void createTextureObject();
  void setupTextureData(const void *data, GLenum internalFormat, GLenum format,
                        GLenum type);
  void loadTexture(const std::string &filePath);
  void setTextureParameters(GLenum mode);

private:
  GLuint mTextureID;
  int mWidth, mHeight, mChannels;
  float mAspectRatio;
  glm::vec2 mSize;
  std::string mFilePath;
};
