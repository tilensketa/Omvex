#pragma once

#include "Managers/TextureManager.h"

#include <string>

class TutorialLoader {
public:
  TutorialLoader(const std::string &resourceFolder, TextureManager *texMng);

  void ShowTutorialWindow();
  void SetTutorial() { mShow = true; }

private:
  void showImageCentered(Texture *texture);

private:
  bool mShow = false;

  std::string mResourceFolder;
  TextureManager* mTextureManager;
  std::vector<Texture *> mTextures;
  std::vector<std::string> mLines;
};
