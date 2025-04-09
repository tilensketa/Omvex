#pragma once

#include <glm/glm.hpp>
#include <imgui.h>
#include <memory>
#include <queue>

#include "CameraParametersManager.h"
#include "TextureManager.h"

#include "Colors.h"

class CameraCalibrator {
public:
  CameraCalibrator(const std::string &baseFolder);

  void Update();
  void SetActiveFolder(std::shared_ptr<std::string> &activeFolder) {
    mActiveFolder = activeFolder;
  }
  void SetTextureManager(std::shared_ptr<TextureManager> &textureManager) {
    mTextureManager = textureManager;
  }
  void SetIsLoading(std::shared_ptr<bool> loading) { mIsLoading = loading; }
  void SetLoadingFracture(std::shared_ptr<float> fracture) {
    mLoadingFracture = fracture;
  }

private:
  void draw();
  void move(int id);
  void drag();

  // opencv need to go
  void recalculateGridPoints();
  glm::vec2 projectPoint3DTo2D(const glm::vec3 &point3D);
  void calculateRotationTranslation();
  void recalculate();

  void handleOpenImage();
  void handleOpenParams();
  void saveParams();

  void onParamChange();
  void handleLoad();
  void addParams(const std::string &paramFile);
  void addImage(const std::string &imageFile);

private:
  char mRCImageDrag[4];
  float mRadius = 10.0f;
  int mChanged = 5;

  std::shared_ptr<TextureManager> mTextureManager;
  std::shared_ptr<Texture> mTexture = nullptr;
  std::string mLoadedImageFilename = "";
  glm::vec2 mImageSize = glm::vec2(0);

  std::shared_ptr<std::string> mActiveFolder = nullptr;
  std::string mExampleFolderPath = "";

  std::shared_ptr<CameraParameters> mCameraParameters = nullptr;
  CameraParametersManager mCameraParametersManager;

  std::vector<ImU32> mCSColors = {Colors::YELLOW, Colors::RED, Colors::GREEN,
                                  Colors::BLUE};
  std::vector<ImU32> mRCColors = {Colors::BLUE, Colors::YELLOW, Colors::CYAN,
                                  Colors::MAGENTA};

  std::queue<std::string> mCameraParamsLoadingQueue;
  std::queue<std::string> mCameraImagesLoadingQueue;
  std::shared_ptr<bool> mIsLoading;
  std::shared_ptr<float> mLoadingFracture;
  int mLoadingAll = 0;
};
