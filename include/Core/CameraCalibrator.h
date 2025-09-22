#pragma once

#include <glm/glm.hpp>
#include <imgui.h>
#include <queue>

#include "Managers/CameraParametersManager.h"
#include "Managers/TextureManager.h"

#include "Core/IAppMode.h"
#include "Core/Loader/ExampleLoader.h"

struct Line {
  ImVec2 start;
  ImVec2 end;
  ImU32 color;
};
struct Circle {
  ImVec2 center;
  float radius;
  ImU32 color;
};

class CameraCalibrator : public IAppMode {
public:
  CameraCalibrator(BaseFolders *folders);

  void OnActivate() override;
  void OnDeactivate() override;
  void Update() override;
  void Render() override;
  void RenderUI() override;

  void SetTextureManager(TextureManager *tm) { mTextureManager = tm; }
  void SetExampleLoader(ExampleLoader *el) { mExampleLoader = el; }

private:
  void drag();

  void handleMain();
  void updateMain(const ImVec2 &windowSize);
  void renderMain();

  void handleSettings();
  void renderSettings();

  void renderDebug();

  void handleOpenImage();
  void handleOpenParams();

  void loadExample();

  void onParamChange();
  void handleLoad();
  void addParams(const std::string &paramFile);
  void addImage(const std::string &imageFile);

  void renderOverlay(ImVec2 imagePos);
  void updateOverlayGeometry(ImVec2 imagePos);
  ImVec2 toGlobalPos(const glm::vec2 &normPos, const ImVec2 &imagePos) const;

private:
  BaseFolders *mBaseFolders = nullptr;
  ExampleLoader *mExampleLoader = nullptr;
  TextureManager *mTextureManager = nullptr;
  Texture *mTexture = nullptr;

  CameraParameters *mCameraParameters = nullptr;
  CameraParametersManager mCameraParametersManager;

  ImVec2 mImageOffset;
  ImVec2 mLastImagePos;
  glm::vec2 mImageSize = glm::vec2(0);

  std::array<bool, 4> mRectImageDrag;
  float mRadius = 10.0f;
  std::vector<Line> mLines;
  std::vector<Circle> mCircles;
  bool mChanged = true;

  std::queue<std::string> mCameraParamsLoadingQueue;
  std::queue<std::string> mCameraImagesLoadingQueue;
};
