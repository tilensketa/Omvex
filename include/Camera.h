#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "CameraParameters.h"
#include "Shader.h"

class Camera {
  enum class Mode { ORBIT };

public:
  Camera(int width, int height, glm::vec3 position);

  void UpdateMatrix();
  void Matrix(Shader &shader, const char *uniform);
  void Inputs(GLFWwindow *window, float ts);
  void OnResize(const glm::vec2 &newResolution);

  const glm::vec3 &GetPosition() const { return mPosition; }
  const float &GetAspectRatio() const { return mAspectRatio; }
  const glm::ivec2 &GetResolution() const { return mResolution; }
  const glm::vec3 &GetUp() const { return mUp; }
  const glm::vec3 &GetTarget() const { return mTarget; }
  const bool &GetStatic() const { return mStatic; }
  const glm::mat4 &GetView() const { return mView; }
  const glm::mat4 &GetProjection() const { return mProjection; }
  const std::string &GetBgImage() const { return mBgImage; }
  const glm::mat4 &GetMatrix() const { return mMatrix; }
  const glm::vec3 GetYawPitchRoll() const {
    return glm::vec3(mYaw, mPitch, mRoll);
  }

  void SetPosition(const glm::vec3 &position) { mPosition = position; }

  void SetStatic(bool stati) { mStatic = stati; }

  void SetParameters(const CameraParameters &params);
  const CameraParameters &GetParameters() const { return *mParameters; }

private:
  void updateOrbitViewMatrix();

  void move(glm::vec3 delta);
  void rotate(float deltaYaw, float deltaPitch);
  void zoom(float delta);
  void pan(float deltaX, float deltaY);

private:
  glm::ivec2 mResolution;
  float mFOV;
  glm::vec3 mPosition;
  glm::vec3 mTarget;
  glm::vec3 mUp;
  float mAspectRatio;
  float mNearPlane = 0.1f;
  float mFarPlane = 1000.0f;

  glm::mat4 mMatrix;
  glm::mat4 mView;
  glm::mat4 mProjection;

  float mYaw, mPitch, mRoll;
  float mMoveSpeed = 30;
  float mRotateSpeed = 1.0f;
  float mSensitivity = 0.1f;
  Mode mMode = Mode::ORBIT;

  // glm::vec2 mLastMousePosition;
  // Mouse state
  double mLastMouseX = 0.0, mLastMouseY = 0.0;
  bool mFirstMouse = true;
  bool mLeftMousePressed = false;
  bool mRightMousePressed = false;

  bool mStatic = false;

  std::string mBgImage = "";
  std::unique_ptr<CameraParameters> mParameters;
};
