#include "CameraParametersManager.h"
#include "FileSystem.h"
#include "Logger.h"

// If isParam -> path = paramPath, else path = imagePath
void CameraParametersManager::AddParameter(const std::string &path,
                                           bool isParam) {
  std::shared_ptr<CameraParameters> param =
      std::make_shared<CameraParameters>("");
  if (isParam) {
    param->Load(path);
  } else {
    // Get param path and ref image path
    param->RefImageFilePath = FileSystem::GetFileNameFromPath(path);

    std::string fileName = FileSystem::GetFileNameFromPath(path);
    std::string saveFolder = FileSystem::GetDirectoryFromPath(path);
    std::string saveName = FileSystem::RemoveFileExtension(fileName) + ".json";
    std::string savePath = saveFolder + "/" + saveName;
    param->Path = savePath;
  }
  for (size_t i = 0; i < mCameraParameters.size(); i++) {
    std::shared_ptr<CameraParameters> cameraParam = mCameraParameters[i];
    if (cameraParam->RefImageFilePath == param->RefImageFilePath) {
      if (cameraParam->Loaded) {
        Logger::getInstance().Debug("Camera parameters already loaded: " +
                                    param->Path);
        mSelectedParameterId = i;
        return;
      } else {
        mSelectedParameterId = i;
        Remove();
      }
    }
  }
  Logger::getInstance().Success("Camera parameters added: " + param->Path);
  mCameraParameters.push_back(param);
  mParameterNames.push_back(param->Path + " / " + param->RefImageFilePath);
  mSelectedParameterId = mCameraParameters.size() - 1;
}

void CameraParametersManager::Remove() {
  if (mSelectedParameterId == -1 || mCameraParameters.size() == 0) {
    Logger::getInstance().Debug("Can't remove parameters");
    return;
  }
  mCameraParameters.erase(mCameraParameters.begin() + mSelectedParameterId);
  mParameterNames.erase(mParameterNames.begin() + mSelectedParameterId);
  if (mCameraParameters.size() > 0) {
    mSelectedParameterId = 0;
  } else {
    mSelectedParameterId = -1;
  }
  Logger::getInstance().Success("Removed camera parameter");
  mChange = true;
}

bool CameraParametersManager::ShowParameters() {
  ImGui::Text("Camera Parameters");
  int prevSelectedId = mSelectedParameterId;
  if (ImGui::BeginListBox(
          "##CameraParameters",
          ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
    for (size_t n = 0; n < mCameraParameters.size(); n++) {
      const bool is_selected = (mSelectedParameterId == n);
      if (ImGui::Selectable(mParameterNames[n].c_str(), is_selected) &&
          mSelectedParameterId != n) {
        mSelectedParameterId = n;
      }
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndListBox();
  }
  bool change = mChange;
  mChange = false;
  return prevSelectedId != mSelectedParameterId || change;
}

void CameraParametersManager::SetDimImage(bool dim) {
  for (std::shared_ptr<CameraParameters> param : mCameraParameters) {
    param->DimImage = dim;
  }
}
void CameraParametersManager::SetShowGrid(bool show) {
  for (std::shared_ptr<CameraParameters> param : mCameraParameters) {
    param->ShowGrid = show;
  }
}
