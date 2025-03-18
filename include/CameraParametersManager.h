#pragma once

#include "CameraParameters.h"
#include <memory>

class CameraParametersManager {
public:
  void AddParameter(const std::string &path, bool isParam);
  void Remove();

  void SetDimImage(bool dim);
  void SetShowGrid(bool show);

  bool ShowParameters();

  const int GetSelectedParameterId() const { return mSelectedParameterId; }
  std::shared_ptr<CameraParameters> GetCameraParameter() {
    if (mSelectedParameterId == -1)
      return nullptr;
    return mCameraParameters[mSelectedParameterId];
  }
  const std::vector<std::shared_ptr<CameraParameters>> &GetCameraParameters() {
    return mCameraParameters;
  }

private:
  std::vector<std::shared_ptr<CameraParameters>> mCameraParameters;
  std::vector<std::string> mParameterNames;
  int mSelectedParameterId = -1;
  int mChange = false;
};
