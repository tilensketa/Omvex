#include "Camera.h"
#include "Utils.h"

Camera::Camera(int width, int height, glm::vec3 position) {
  mResolution = glm::ivec2(width, height);
  mFOV = 45.0f;
  mPosition = position;
  mFront = glm::vec3(-1, 0, 0);
  mUp = glm::vec3(0, 0, 1);
  mAspectRatio = (float)mResolution.x / (float)mResolution.y;
}

void Camera::UpdateMatrix() {
  if (mStatic)
    return;

  // Create view matrix
  mView = glm::lookAt(mPosition, mPosition + mFront, mUp);

  float mNearPlane = 0.1f;
  float mFarPlane = 1000.0f;

  glm::mat4 projection =
      glm::perspective(glm::radians(mFOV), (float)mResolution.x / mResolution.y,
                       mNearPlane, mFarPlane);

  mMatrix = projection * mView;
}

void Camera::Matrix(Shader &shader, const char *uniform) {
  shader.SetMat4(uniform, mMatrix);
}

void Camera::Inputs(GLFWwindow *window, float ts) {
  if (mStatic)
    return;
  double x;
  double y;
  glfwGetCursorPos(window, &x, &y);
  glm::vec2 mousePos(x, y);
  glm::vec2 delta = (mousePos - mLastMousePosition) * 0.002f;
  mLastMousePosition = mousePos;

  if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    return;
  }

  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glm::vec3 right = glm::cross(mFront, mUp);

  // Position
  if (glfwGetKey(window, GLFW_KEY_W)) {
    mPosition += mFront * mMoveSpeed * ts;
  } else if (glfwGetKey(window, GLFW_KEY_S)) {
    mPosition -= mFront * mMoveSpeed * ts;
  }

  if (glfwGetKey(window, GLFW_KEY_A)) {
    mPosition -= right * mMoveSpeed * ts;
  } else if (glfwGetKey(window, GLFW_KEY_D)) {
    mPosition += right * mMoveSpeed * ts;
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE)) {
    mPosition += mUp * mMoveSpeed * ts;
  } else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
    mPosition -= mUp * mMoveSpeed * ts;
  }

  // Rotation
  if (delta.x != 0.0f || delta.y != 0.0f) {
    float pitchDelta = delta.y * mRotateSpeed;
    float yawDelta = delta.x * mRotateSpeed;

    glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, right),
                                            glm::angleAxis(-yawDelta, mUp)));
    mFront = glm::rotate(q, mFront);
    UpdateMatrix();
  }
}

void Camera::OnResize(const glm::vec2 &newResolution) {
  mResolution = newResolution;
  UpdateMatrix();
}
void Camera::SetTransformation(const glm::vec3 &translation,
                               const glm::mat3 &rotation) {
  mStatic = true;

  glm::vec3 mPosition = translation;

  glm::mat3 Rt = glm::transpose(rotation);

  glm::mat4 view = glm::mat4(0.0f);
  view[0] = glm::vec4(rotation[0], 0.0f); // First row (right vector)
  view[1] = glm::vec4(rotation[1], 0.0f); // Second row (up vector)
  view[2] = glm::vec4(rotation[2], 0.0f); // Third row (forward vector)
  view[3] = glm::vec4(translation, 1.0f); // Translation
                                          //

  glm::mat4 projection = glm::mat4(1.0f);
  float mNearPlane = 0.1f;
  float mFarPlane = 1000.0f;
  projection =
      glm::perspective(glm::radians(mFOV), (float)mResolution.x / mResolution.y,
                       mNearPlane, mFarPlane);

  mMatrix = projection * view;
}

void Camera::SetParameters(const CameraParameters &params) {
  mStatic = true;

  glm::vec3 translation = params.Translation;
  glm::mat3 rotation = params.Rotation;
  glm::mat3 intrinsics = params.Intrinsic;
  glm::ivec2 imageSize = params.ImageCalibratedSize;
  float fx = intrinsics[0][0];
  float fy = intrinsics[1][1];
  float cx = intrinsics[2][0];
  float cy = intrinsics[2][1];

  float nearPlane = 0.1f;
  float farPlane = 1000.0f;

  mPosition = translation;
  mPosition[2] *= -1.0f;

  mFront = rotation[2];
  mFront[2] *= -1.0f;
  // mUp = glm::vec3(0,0,1);
  mUp = rotation[1];
  mUp[2] *= -1.0f;
  // mUp = glm::vec3(0,0,1);

  glm::mat4 proj = glm::mat4(0.0f);
  proj[0][0] = 2.0f * fx / imageSize.x;
  proj[1][1] = 2.0f * fy / imageSize.y;
  proj[2][0] = 1.0f - 2.0f * cx / imageSize.x;
  proj[2][1] = 2.0f * cy / imageSize.y - 1.0f;
  proj[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
  proj[2][3] = -1.0f;
  proj[3][2] = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);

  mBgImage = params.RefImageFilePath;

  mView = glm::lookAt(mPosition, mPosition + mFront, mUp);
  mMatrix = proj * mView;
}
