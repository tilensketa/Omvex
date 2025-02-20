#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Texture::Texture(const std::string &filePath) {
  mFilePath = filePath;
  mTextureID = loadTexture(filePath);
  mSize = glm::vec2(static_cast<float>(mWidth), static_cast<float>(mHeight));
  mAspectRatio = mSize.x / mSize.y;
}

Texture::Texture(int width, int height) {
  mWidth = width;
  mHeight = height;
  mChannels = 3; // Assuming RGB
  mSize = glm::vec2(static_cast<float>(mWidth), static_cast<float>(mHeight));
  mAspectRatio = mSize.x / mSize.y;

  // Generate OpenGL texture
  glGenTextures(1, &mTextureID);
  glBindTexture(GL_TEXTURE_2D, mTextureID);

  // Allocate empty texture data
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, nullptr);

  // Set texture parameters
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() { glDeleteTextures(1, &mTextureID); }

void Texture::Bind() const {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

GLuint Texture::loadTexture(const std::string &filePath) {
  stbi_set_flip_vertically_on_load(false);

  // Load image data
  unsigned char *data =
      stbi_load(filePath.c_str(), &mWidth, &mHeight, &mChannels, 0);
  if (!data) {
    Logger::getInstance().Log("Failed to load texture: " + filePath,
                              LogLevel::FATAL);
  } else {
    Logger::getInstance().Log("Successfully loaded texture: " + filePath,
                              LogLevel::SUCCESS);
  }

  // Generate OpenGL texture
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Load the texture into OpenGL
  GLint internalFormat = (mChannels == 4) ? GL_RGBA : GL_RGB;
  GLenum format = (mChannels == 4) ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Free the image data
  stbi_image_free(data);

  return texture;
}

void Texture::Save(const std::string &path) {
  Bind();

  // Always allocate space for RGBA (4 channels), even if the original was RGB
  int imageSize = mWidth * mHeight * 4;
  unsigned char *data = new unsigned char[imageSize];

  // Read pixels from OpenGL (forcing RGBA format)
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  // Save as PNG using stb_image_write
  if (stbi_write_png(path.c_str(), mWidth, mHeight, 4, data, mWidth * 4)) {
    Logger::getInstance().Log("Texture saved to: " + path, LogLevel::SUCCESS);
  } else {
    Logger::getInstance().Log("Failed to save texture: " + path,
                              LogLevel::FATAL);
  }

  // Cleanup
  delete[] data;
  Unbind();
}
