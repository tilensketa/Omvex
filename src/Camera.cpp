#include "Camera.h"
#include "Logger.h"
#include "Utils.h"

Camera::Camera(int width, int height, glm::vec3 position) {
  mResolution = glm::ivec2(width, height);
  mFOV = 45.0f;
  mPosition = position;
  mTarget = glm::vec3(0);
  mUp = glm::vec3(0, 0, 1);
  mAspectRatio = (float)mResolution.x / (float)mResolution.y;
  mYaw = 0.0f;
  mPitch = 0.0f;
  mRoll = 0.0f;
}

void Camera::UpdateMatrix() {
  if (mStatic)
    return;

  if (mMode == Mode::ORBIT) {
    updateOrbitViewMatrix();
  }
  mProjection =
      glm::perspective(glm::radians(mFOV), mAspectRatio, mNearPlane, mFarPlane);

  mMatrix = mProjection * mView;
}
void Camera::updateOrbitViewMatrix() {
  glm::vec3 direction =
      glm::vec3(cos(glm::radians(mYaw)) * cos(glm::radians(mPitch)),
                sin(glm::radians(mYaw)) * cos(glm::radians(mPitch)),
                sin(glm::radians(mPitch)));

  mPosition = mTarget - direction * glm::distance(mPosition, mTarget);
  mView = glm::lookAt(mPosition, mTarget, mUp);
}

void Camera::Matrix(Shader &shader, const char *uniform) {
  shader.SetMat4(uniform, mMatrix);
}

void Camera::Inputs(GLFWwindow *window, float ts) {
  float speed = mMoveSpeed * ts;

  // Mouse input
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    if (mFirstMouse) {
      mLastMouseX = xpos;
      mLastMouseY = ypos;
      mFirstMouse = false;
    }

    float dx = static_cast<float>(xpos - mLastMouseX);
    float dy = static_cast<float>(ypos - mLastMouseY);
    rotate(-dx, dy);
  } else {
    mFirstMouse = true;
  }

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    float dx = static_cast<float>(xpos - mLastMouseX) * 0.01f;
    float dy = static_cast<float>(ypos - mLastMouseY) * 0.01f;
    pan(dx, dy);
  }

  mLastMouseX = xpos;
  mLastMouseY = ypos;

  // Scroll input for zoom
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    zoom(mSensitivity);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    zoom(-mSensitivity);
}

void Camera::OnResize(const glm::vec2 &newResolution) {
  mResolution = newResolution;
  UpdateMatrix();
}

void Camera::SetParameters(const std::string &folderPath,
                           const CameraParameters &params) {
  Logger::getInstance().Debug("Camera set parameters " + folderPath);
  mParameters = std::make_unique<CameraParameters>(params);
  mStatic = true;

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
  float aspectRatio =
      static_cast<float>(imageSize.x) / static_cast<float>(imageSize.y);
  mProjection = glm::perspective(fovY, aspectRatio, mNearPlane, mFarPlane);

  mBgImage = folderPath + "/" + params.RefImageFilePath;

  mMatrix = mProjection * mView;
}

// Move camera
void Camera::move(glm::vec3 delta) {
  if (mMode == Mode::ORBIT) {
    mTarget += delta;
  }
  UpdateMatrix();
}

// Rotate camera
void Camera::rotate(float deltaYaw, float deltaPitch) {
  mYaw += deltaYaw * mSensitivity;
  mPitch += deltaPitch * mSensitivity;

  // Clamp pitch to prevent flipping
  mPitch = glm::clamp(mPitch, -80.0f, 80.0f);

  UpdateMatrix();
}

// Zoom camera
void Camera::zoom(float delta) {
  if (mMode == Mode::ORBIT) {
    glm::vec3 direction = glm::normalize(mTarget - mPosition);
    mPosition += direction * delta * mMoveSpeed;
  }
  UpdateMatrix();
}

// Pan camera (move horizontally/vertically)
void Camera::pan(float deltaX, float deltaY) {
  glm::vec3 right = glm::normalize(glm::cross(mTarget - mPosition, mUp));
  glm::vec3 upMove = glm::normalize(glm::cross(right, mTarget - mPosition));

  if (mMode == Mode::ORBIT) {
    mTarget += (right * deltaX + upMove * deltaY) * mMoveSpeed;
    mPosition += (right * deltaX + upMove * deltaY) * mMoveSpeed;
  }
  UpdateMatrix();
}
