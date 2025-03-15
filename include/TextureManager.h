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
      Logger::getInstance().Info("Texture already loaded: " + path);
      return mTextures[it->second];
    }

    // Load the texture if not already loaded
    Logger::getInstance().Info("Loading and caching new texture: " + path);
    auto texture = std::make_shared<Texture>(path);
    mTextures.push_back(texture);
    mTextureMap[path] = mTextures.size() - 1;
    return texture;
  }

private:
  std::vector<std::shared_ptr<Texture>> mTextures;
  std::unordered_map<std::string, size_t> mTextureMap;
};
