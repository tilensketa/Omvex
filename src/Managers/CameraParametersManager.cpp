#include "Managers/CameraParametersManager.h"

#include "Core/Logger.h"
#include "Utilities/ImGuiHelpers.h"

#include <imgui.h>

void CameraParametersManager::AddParameter(const std::string &paramPath) {
  Logger::Info("CameraParametersManager: Adding camera parameters " +
               paramPath);
  auto param = std::make_unique<CameraParameters>();
  param->LoadJson(paramPath);
  add(std::move(param));
}
void CameraParametersManager::AddImage(const std::string &imagePath) {
  Logger::Info("CameraParametersManager: Adding image " + imagePath);
  auto param = std::make_unique<CameraParameters>();
  param->LoadImage(imagePath);
  add(std::move(param));
}
void CameraParametersManager::add(std::unique_ptr<CameraParameters> param) {
  for (size_t i = 0; i < mCameraParameters.size(); i++) {
    auto &cameraParam = mCameraParameters[i];
    if (cameraParam->RefImageFileName == param->RefImageFileName) {
      mSelectedId = static_cast<int>(i);
      Logger::Debug("Camera parameters already loaded: " + param->Path);
      return;
    }
  }
  mParameterNames.push_back(param->Path + " / " + param->RefImageFileName);
  mCameraParameters.push_back(std::move(param));
  mSelectedId = static_cast<int>(mCameraParameters.size()) - 1;
  Logger::Success("CameraParametersManager: Added camera parameters");
}

void CameraParametersManager::Remove(int id) {
  if (!isIdValid(id)) return;

  mCameraParameters.erase(mCameraParameters.begin() + id);
  mParameterNames.erase(mParameterNames.begin() + id);

  // Calculate new selected id
  if (!mCameraParameters.empty()) {
    mSelectedId =
        std::min(mSelectedId, static_cast<int>(mCameraParameters.size() - 1));
  } else {
    mSelectedId = -1;
  }
  Logger::Info("CameraParametersManager: Removed camera parameter");
  mChange = true;
}

CameraParameters *CameraParametersManager::GetCameraParameter() {
  if (!isIdValid(mSelectedId)) return nullptr;
  return mCameraParameters[mSelectedId].get();
}
bool CameraParametersManager::isIdValid(int id) {
  return (id >= 0 && id < static_cast<int>(mCameraParameters.size()));
}

bool CameraParametersManager::ShowParameters() {
  ImGuiHelpers::ShowSelectableList("CameraParameters", mParameterNames,
                                   mSelectedId, [&](int) { mChange = true; });
  bool change = mChange;
  mChange = false;
  return change;
}

void CameraParametersManager::SetDimImage(bool dim) {
  for (auto &param : mCameraParameters) {
    param->DimImage = dim;
  }
}
void CameraParametersManager::SetShowGrid(bool show) {
  for (auto &param : mCameraParameters) {
    param->ShowGrid = show;
  }
}

void CameraParametersManager::SaveSelected() {
  if (isIdValid(mSelectedId)) mCameraParameters[mSelectedId]->Save();
}

void CameraParametersManager::SaveAll() {
  for (auto &param : mCameraParameters)
    param->Save();
}
