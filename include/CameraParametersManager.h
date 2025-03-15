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
    // Load the camera parameters if not already loaded
    Logger::getInstance().Info("Loading and caching new camera parameteres: " + paramPath);
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
