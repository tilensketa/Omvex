#pragma once

#include <glm/glm.hpp>
#include <imgui.h>
#include <memory>
#include <vector>

#include "CameraParametersManager.h"
#include "TextureManager.h"

class CameraCalibrator {
public:
  CameraCalibrator();

  void Update();

private:
  void draw();
  void move(int id);
  void drag();

  void recalculateGridPoints();
  glm::vec2 projectPoint3DTo2D(const glm::vec3 &point3D);
  void calculateRotationTranslation();
  void recalculate();

  void handleOpenImage();
  void handleOpenParams();

private:
  char mRCImageDrag[4];
  float mRadius = 10.0f;
  bool mChanged = true;

  TextureManager mTextureManager;
  std::shared_ptr<Texture> mTexture = nullptr;
  std::string mLoadedImageFilename = "";
  glm::vec2 mImageSize;

  std::string mFolderPath = "/home/tilen/dev/Omvex/images/bg1.jpg";

  std::unique_ptr<CameraParameters> mCameraParameters = nullptr;
};
