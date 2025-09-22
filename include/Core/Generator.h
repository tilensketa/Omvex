#pragma once

#include "Managers/CameraManager.h"
#include "Managers/ModelManager.h"
#include "Managers/PhysicsManager.h"

#include <chrono>
#include <glm/glm.hpp>
#include <string>

enum ViewMode { Color = 0, Segmentation, Count };

class Generator {
public:
  Generator(CameraManager *camMgr, ModelManager *modelMng,
            PhysicsManager *phyMng, ViewMode *viewMode);

  void Start(const std::string &outputFolder);
  void Stop();
  void Update();

  bool IsRunning() const { return mRunning; }

  int &ModifyNumRenders() { return mNumRenders; }
  float GetProgress() { return float(mRenderId) / float(mNumRenders + 1); }
  bool NeedSim() {
    return (mRenderId % mCameraManager->GetCount() == 0 || mRenderId == 0) &&
           mRenderSubId == 0;
  }

private:
  std::string getSubfolder() const;
  std::string getFileName() const;
  void saveImage();
  void saveTransforms();

private:
  CameraManager *mCameraManager;
  ModelManager *mModelManager;
  PhysicsManager *mPhysicsManager;
  ViewMode *mViewMode;

  std::string mOutputFolder;

  int mRenderId = 0;
  int mRenderSubId = 0;
  int mNumRenders = 10;
  bool mRunning = false;

  std::chrono::high_resolution_clock::time_point mStartTime;
};
