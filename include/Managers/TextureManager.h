#pragma once

#include "Core/Logger.h"
#include "Rendering/Textures/Texture.h"
#include <memory>
#include <unordered_map>
#include <vector>

class TextureManager {
public:
  // Load or retrieve a texture
  Texture *GetTexture(const std::string &path) {
    // Check if the texture is already loaded
    auto it = mTextureMap.find(path);
    if (it != mTextureMap.end()) {
      // Logger::Debug("Texture already loaded: " + path);
      return mTextures[it->second].get();
    }

    // Load the texture if not already loaded
    Logger::Info("TextureManager: Loading and caching new texture: " + path);
    auto texture = std::make_unique<Texture>(path);
    Texture *rawTexture = texture.get();
    mTextures.push_back(std::move(texture));
    mTextureMap[path] = mTextures.size() - 1;
    return rawTexture;
  }

private:
  std::vector<std::unique_ptr<Texture>> mTextures;
  std::unordered_map<std::string, size_t> mTextureMap;
};
