#include "Managers/CameraManager.h"

#include "Utilities/FileSystem.h"
#include "Utilities/ImGuiHelpers.h"

#include <imgui.h>

CameraManager::CameraManager() {}

void CameraManager::AddCameraFBO(const std::string &paramPath, int height) {
  CameraParameters params;
  params.LoadJson(paramPath);
  std::string folderPath = FileSystem::GetDirectoryFromPath(paramPath);
  glm::ivec2 resolution = params.ImageCalibratedSize;
  float aspectRatio = float(resolution.x) / resolution.y;
  int width = static_cast<int>(height * aspectRatio);
  std::unique_ptr<Camera> camera =
      std::make_unique<Camera>(width, height, glm::vec3(0.0f));
  camera->SetParameters(folderPath, params);
  std::unique_ptr<FBO> fbo = std::make_unique<FBO>(width, height);

  std::string name = "Camera " + std::to_string(GetCount()) + " " + paramPath;
  mCameras.push_back(std::move(camera));
  mCameraNames.push_back(name);
  mFrameBuffers.push_back(std::move(fbo));
  mSelectedId++;
  mSwitched = true;
  Logger::Success("Added camera: " + name);
}

bool CameraManager::isIdValid(int id) {
  return (id >= 0 && id < static_cast<int>(mCameras.size()) &&
          id < static_cast<int>(mFrameBuffers.size()));
}

Camera *CameraManager::GetCamera() {
  if (!isIdValid(mSelectedId)) return nullptr;
  return mCameras[mSelectedId].get();
}
FBO *CameraManager::GetFBO() {
  if (!isIdValid(mSelectedId)) return nullptr;
  return mFrameBuffers[mSelectedId].get();
}

bool CameraManager::HandleSwitching() {
  bool prevSwitch = mSwitched;
  mSwitched = false;
  return prevSwitch;
}

void CameraManager::SwitchPrevious() {
  mSelectedId = (mSelectedId - 1 + mCameras.size()) % mCameras.size();
  mSwitched = true;
}
void CameraManager::SwitchNext() {
  mSelectedId = (mSelectedId + 1) % mCameras.size();
  mSwitched = true;
}

void CameraManager::Remove(int id) {
  if (!isIdValid(id)) return;

  mCameras.erase(mCameras.begin() + id);
  mFrameBuffers.erase(mFrameBuffers.begin() + id);
  mCameraNames.erase(mCameraNames.begin() + id);

  if (!mCameras.empty()) {
    mSelectedId = std::min(mSelectedId, static_cast<int>(mCameras.size() - 1));
  } else {
    mSelectedId = -1;
  }
  mSwitched = true;
  Logger::Success("Removed camera");
}

void CameraManager::ChangeResolution(int height) {
  for (int i = 0; i < GetCount(); i++) {
    std::unique_ptr<FBO> &fbo = mFrameBuffers[i];
    std::unique_ptr<Camera> &camera = mCameras[i];
    float aspectRatio = camera->GetAspectRatio();
    int width = static_cast<int>(height * aspectRatio);
    fbo->Resize(width, height);
    camera->SetResolution(glm::ivec2(width, height));
  }
}

void CameraManager::ShowCameras() {
  ImGuiHelpers::ShowSelectableList("Cameras", mCameraNames, mSelectedId,
                                   [&](int) { mSwitched = true; });
}
