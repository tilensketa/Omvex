#pragma once

#include "Core/Camera/Camera.h"
#include "Rendering/Buffers/FBO.h"

#include <memory>
#include <vector>

class CameraManager {
public:
  CameraManager();

  void AddCameraFBO(const std::string &paramPath, int height);
  void Remove(int id);

  bool HandleSwitching();
  void SwitchNext();
  void SwitchPrevious();
  void ChangeResolution(int height);

  void ShowCameras();

  Camera *GetCamera();
  FBO *GetFBO();
  const int GetCount() const { return static_cast<int>(mCameras.size()); }

  const int GetSelectedId() const { return mSelectedId; }

  std::vector<std::unique_ptr<FBO>> &GetFBOs() { return mFrameBuffers; }
  std::vector<std::unique_ptr<Camera>> &GetCameras() { return mCameras; }

private:
  bool isIdValid(int id);

private:
  std::vector<std::unique_ptr<Camera>> mCameras;
  std::vector<std::unique_ptr<FBO>> mFrameBuffers;
  std::vector<std::string> mCameraNames;
  int mSelectedId = -1;
  bool mSwitched = true;
};
