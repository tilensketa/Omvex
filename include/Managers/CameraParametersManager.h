#pragma once

#include "Core/Camera/CameraParameters.h"

#include <memory>

class CameraParametersManager {
public:
  void AddParameter(const std::string &paramPath);
  void AddImage(const std::string &imagePath);
  void Remove(int id);

  void SetDimImage(bool dim);
  void SetShowGrid(bool show);

  bool ShowParameters();

  void SaveSelected();
  void SaveAll();
  CameraParameters *GetCameraParameter();
  int GetSelectedId() const { return mSelectedId; }

private:
  void add(std::unique_ptr<CameraParameters> param);
  bool isIdValid(int id);

private:
  std::vector<std::unique_ptr<CameraParameters>> mCameraParameters;
  std::vector<std::string> mParameterNames;
  int mSelectedId = -1;
  bool mChange = false;
};
