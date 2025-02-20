#pragma once

#include "Logger.h"
#include "Texture.h"
#include <memory>
#include <unordered_map>

class TextureManager {
public:
  // Load or retrieve a texture
  std::shared_ptr<Texture> getTexture(const std::string &path) {
    // Check if the texture is already loaded
    auto it = mTextureMap.find(path);
    if (it != mTextureMap.end()) {
      std::string msg = "Texture already loaded: " + path;
      Logger::getInstance().Log(msg, LogLevel::INFO);
      return mTextures[it->second];
    }

    // Load the texture if not already loaded
    std::string msg = "Loading and caching new texture: " + path;
    Logger::getInstance().Log(msg, LogLevel::INFO);
    auto texture = std::make_shared<Texture>(path);
    mTextures.push_back(texture);
    mTextureMap[path] = mTextures.size() - 1;
    return texture;
  }

private:
  std::vector<std::shared_ptr<Texture>> mTextures;
  std::unordered_map<std::string, size_t> mTextureMap;
};
