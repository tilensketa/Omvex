#pragma once

#include "CameraManager.h"
#include "FBO.h"
#include <string>

enum ViewMode { Flat = 0, Shaded, Segmented };

class RenderManager {
public:
  void Update(std::shared_ptr<FBO> frameBuffer, bool canRender);
  void UpdateRenderClick(const std::string &folderPath);

  void SetViewMode(ViewMode *viewMode) { mViewMode = viewMode; }
  void SetCameraManager(CameraManager *cameraManager) {
    mCameraManager = cameraManager;
  }

  int &ModRenders() { return mRenders; }

  bool IsRendering() { return mRendering; }
  int GetRenderId() { return mRenderId; }
  int GetRenderSubId() { return mRenderSubId; }
  int GetRenders() { return mRenders; }

private:
  void render(std::shared_ptr<FBO> frameBuffer);
  void saveImage(std::shared_ptr<FBO> frameBuffer, const std::string &path);

private:
  bool mRendering = false;
  int mRenderId = 0;
  int mRenderSubId = 0;
  std::string mRenderFolder = "";
  int mRenders = 5;
  bool mRenderSetup = false;

  CameraManager *mCameraManager;
  ViewMode *mViewMode;
};
