#pragma once

#include "Camera.h"
#include "FBO.h"
#include <chrono>
#include <memory>
#include <vector>

class CameraManager {
public:
  CameraManager();

  void AddCamera(std::shared_ptr<Camera> camera);
  void AddFBO(std::shared_ptr<FBO> fbo) { mFrameBuffers.push_back(fbo); }
  bool Remove();

  bool HandleSwitching();
  void SwitchNext();
  void SwitchPrevious();

  void ShowCameras();

  std::shared_ptr<Camera> GetCamera();
  std::shared_ptr<FBO> GetFBO();
  const int GetCount() const { return static_cast<int>(mCameras.size()); }

  const int GetSelectedId() const { return mSelectedCameraId; }

  std::vector<std::shared_ptr<FBO>> &GetFBOs() { return mFrameBuffers; }
  std::vector<std::shared_ptr<Camera>> &GetCameras() { return mCameras; }

private:
  std::vector<std::shared_ptr<Camera>> mCameras;
  std::vector<std::shared_ptr<FBO>> mFrameBuffers;
  std::vector<std::string> mCameraNames;
  int mSelectedCameraId = -1;

  bool mSwitched = true;
};
