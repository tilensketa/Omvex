#include "Rendering/Textures/Texture.h"

#include "Core/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void Texture::initializeCommonMembers(int width, int height, int channels) {
  mWidth = width;
  mHeight = height;
  mChannels = channels;
  mSize = glm::vec2(static_cast<float>(width), static_cast<float>(height));
  mAspectRatio = (height > 0) ? mSize.x / mSize.y : mSize.x;
}
void Texture::createTextureObject() {
  glGenTextures(1, &mTextureID);
  Bind();
}
void Texture::setupTextureData(const void *data, GLenum internalFormat,
                               GLenum format, GLenum type) {
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format,
               type, data);

  // Generate mipmaps only if we have actual texture data (not nullptr)
  if (data != nullptr) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }
}
void Texture::setTextureParameters(GLenum mode) {
  // GL_LINEAR, GL_NEAREST
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

GLenum getFormatFromChannels(int channels) {
  switch (channels) {
  case 4: return GL_RGBA;
  case 3: return GL_RGB;
  case 1: return GL_RED;
  default: return GL_RGBA;
  }
}
GLenum getInternalFormatFromChannels(int channels) {
  switch (channels) {
  case 4: return GL_RGBA8;
  case 3: return GL_RGB8;
  case 1: return GL_R8;
  default: return GL_RGBA8;
  }
}

Texture::Texture(const std::string &filePath) : mFilePath(filePath) {
  loadTexture(filePath);
  mSize = glm::vec2(static_cast<float>(mWidth), static_cast<float>(mHeight));
  mAspectRatio = mSize.x / mSize.y;
}

Texture::Texture(int width, int height, int channels) {
  initializeCommonMembers(width, height, channels);
  createTextureObject();
  setupTextureData(nullptr, getInternalFormatFromChannels(channels),
                   getFormatFromChannels(channels), GL_UNSIGNED_BYTE);
  setTextureParameters(GL_LINEAR);
  Unbind();
}
inline unsigned char FloatToByte(float value) {
  return static_cast<unsigned char>(
      round(glm::clamp(value, 0.0f, 1.0f) * 255.0f));
}
Texture::Texture(const std::vector<glm::vec3> &pixelColors) {
  int width = static_cast<int>(pixelColors.size());
  initializeCommonMembers(width, 1, 4);

  std::vector<unsigned char> imageData(mWidth * 4); // RGBA per pixel

  for (int i = 0; i < mWidth; ++i) {
    glm::vec4 color = glm::vec4(pixelColors[i], 1.0f);
    imageData[i * 4 + 0] = FloatToByte(color.r);
    imageData[i * 4 + 1] = FloatToByte(color.g);
    imageData[i * 4 + 2] = FloatToByte(color.b);
    imageData[i * 4 + 3] = FloatToByte(color.a);
  }
  createTextureObject();
  setupTextureData(imageData.data(), GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
  setTextureParameters(GL_NEAREST);
  Unbind();
}

Texture::~Texture() { glDeleteTextures(1, &mTextureID); }

void Texture::Bind() const {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::loadTexture(const std::string &filePath) {
  stbi_set_flip_vertically_on_load(false);

  // Load image data
  unsigned char *data =
      stbi_load(filePath.c_str(), &mWidth, &mHeight, &mChannels, 0);
  if (!data) {
    Logger::Error("Failed to load texture: " + filePath);
  }

  createTextureObject();
  setTextureParameters(GL_LINEAR);
  setupTextureData(data, getInternalFormatFromChannels(mChannels),
                   getFormatFromChannels(mChannels), GL_UNSIGNED_BYTE);

  stbi_image_free(data);
}

void Texture::Save(const std::string &path) {
  Bind();

  // Always allocate space for RGBA (4 channels), even if the original was RGB
  int imageSize = mWidth * mHeight * 4;
  unsigned char *data = new unsigned char[imageSize];

  // Read pixels from OpenGL (forcing RGBA format)
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  // Save as PNG using stb_image_write
  if (!stbi_write_png(path.c_str(), mWidth, mHeight, 4, data, mWidth * 4)) {
    Logger::Error("Failed to save texture to: " + path);
  }

  // Cleanup
  delete[] data;
  Unbind();
}
