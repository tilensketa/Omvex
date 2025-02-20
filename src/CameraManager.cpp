#include "CameraManager.h"
#include <imgui.h>

CameraManager::CameraManager() {
  mPrevPress = std::chrono::high_resolution_clock::now();
}

std::shared_ptr<Camera> CameraManager::GetCamera() {
  if (GetCount() == 0) {
    return nullptr;
  }
  return mCameras[mSelectedCameraId];
}
std::shared_ptr<FBO> CameraManager::GetFBO() {
  if (GetCount() == 0) {
    return nullptr;
  }
  return mFrameBuffers[mSelectedCameraId];
}

void CameraManager::AddCamera(std::shared_ptr<Camera> camera) {
  mCameras.push_back(camera);
  std::string name = "Camera " + std::to_string(mCameraNames.size() + 1) + " " +
                     camera->GetBgImage() + " (" +
                     std::to_string(camera->GetResolution().x) + ", " +
                     std::to_string(camera->GetResolution().y) + ")";
  mCameraNames.push_back(name);
}

bool CameraManager::HandleSwitching(GLFWwindow *window) {
  /* if (mCameras.size() <= 1) {
    return false;
  } */
  int oldSelectedCameraId = mSelectedCameraId;
  for (int i = 0; i < mCameras.size(); ++i) {
    int key = GLFW_KEY_1 + i;
    if (glfwGetKey(window, key) == GLFW_PRESS) {
      mSelectedCameraId = i;
      mSwitched = true;
    }
  }

  auto now = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = now - mPrevPress;
  if (elapsed.count() >= mCooldown) {
    if (glfwGetKey(window, GLFW_KEY_LEFT)) {
      SwitchPrevious();
      mPrevPress = now;
    } else if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
      SwitchNext();
      mPrevPress = now;
    }
  }
  bool s = mSwitched;
  mSwitched = false;
  return s;
}

void CameraManager::SwitchPrevious() {
  /* if (GetCount() <= 1) {
    mSwitched = false;
    return;
  } */
  mSelectedCameraId -= 1;
  if (mSelectedCameraId < 0) {
    mSelectedCameraId = mCameras.size() - 1;
  }
  mSwitched = true;
}
void CameraManager::SwitchNext() {
  /* if (GetCount() <= 1) {
    mSwitched = false;
    return;
  } */
  mSelectedCameraId += 1;
  if (mSelectedCameraId > mCameras.size() - 1) {
    mSelectedCameraId = 0;
  }
  mSwitched = true;
}

void CameraManager::Remove() {
  if (mCameras.size() > 0 && mFrameBuffers.size() > 0) {
    mCameras.erase(mCameras.begin() + mSelectedCameraId);
    mFrameBuffers.erase(mFrameBuffers.begin() + mSelectedCameraId);
    mCameraNames.erase(mCameraNames.begin() + mSelectedCameraId);
    mSelectedCameraId = 0;
  }
}

void CameraManager::ShowCameras() {
  ImGui::Text("Cameras");
  if (ImGui::BeginListBox(
          "##Cameras",
          ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
    for (int n = 0; n < mCameras.size(); n++) {
      const bool is_selected = (mSelectedCameraId == n);
      if (ImGui::Selectable(mCameraNames[n].c_str(), is_selected) &&
          mSelectedCameraId != n) {
        mSelectedCameraId = n;
        mSwitched = true;
      }
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndListBox();
  }
}
