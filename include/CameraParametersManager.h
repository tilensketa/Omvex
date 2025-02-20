#pragma once

#include "CameraParameters.h"
#include "Logger.h"
#include <memory>
#include <unordered_map>

class CameraParametersManager {
public:
  // Load or retrieve a camera parameters
  std::shared_ptr<CameraParameters>
  getCameraParameters(const std::string &paramPath) {
    // Check if the camera parameters are already loaded
    /* auto it = mParameterMap.find(paramPath);
    if (it != mParameterMap.end()) {
      std::string msg = "Camera parameters already loaded: " + paramPath;
      Logger::getInstance().Log(msg, LogLevel::INFO);
      return mCameraParameters[it->second];
    } */

    // Load the camera parameters if not already loaded
    std::string msg = "Loading and caching new camera parameters: " + paramPath;
    Logger::getInstance().Log(msg, LogLevel::INFO);
    std::shared_ptr<CameraParameters> parameters;
    if (paramPath == "") {
      parameters = std::make_shared<CameraParameters>(paramPath);
    } else {
      /* parameters =
          std::make_shared<CameraParameters>(CameraParameters::Load(paramPath)); */
    }
    mCameraParameters.push_back(parameters);
    mParameterMap[paramPath] = mCameraParameters.size() - 1;
    return parameters;
  }

private:
  std::vector<std::shared_ptr<CameraParameters>> mCameraParameters;
  std::unordered_map<std::string, size_t> mParameterMap;
};
