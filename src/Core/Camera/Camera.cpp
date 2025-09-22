#include "Core/Camera/Camera.h"
#include "Core/Logger.h"

Camera::Camera(int width, int height, glm::vec3 position) {
  mResolution = glm::ivec2(width, height);
  mPosition = position;
  mTarget = glm::vec3(0);
  mUp = glm::vec3(0, 0, 1);
  mAspectRatio = (float)mResolution.x / (float)mResolution.y;
}

// Calculate view and projection matrix from parameters
void Camera::SetParameters(const std::string &folderPath,
                           const CameraParameters &params) {
  Logger::Debug("Camera set parameters " + folderPath);
  mParameters = std::make_unique<CameraParameters>(params);

  glm::vec3 translation = params.Translation;
  glm::mat3 rotation = params.Rotation;
  glm::ivec2 imageSize = params.ImageCalibratedSize;
  float fy = params.Intrinsic[1][1];

  // From OpenCV to OpenGL to myCoord
  mPosition = translation;
  mPosition[2] *= -1.0f;
  glm::vec3 forward = glm::normalize(rotation[2]);
  forward[2] *= -1.0f;
  mUp = glm::normalize(rotation[1]);
  mUp[2] *= -1.0f;
  mTarget = mPosition + forward;

  mView = glm::lookAt(mPosition, mTarget, mUp);

  // Vertical FoV based on intrinsics
  float fovY = 2.0f * atan(imageSize.y / (2.0f * fy));
  mAspectRatio =
      static_cast<float>(imageSize.x) / static_cast<float>(imageSize.y);
  mProjection = glm::perspective(fovY, mAspectRatio, mNearPlane, mFarPlane);

  mBgImage = folderPath + "/" + params.RefImageFileName;

  mMatrix = mProjection * mView;
}

void Camera::SetResolution(const glm::ivec2 &res) {
  mResolution = res;
  mAspectRatio = (float)mResolution.x / (float)mResolution.y;
}
