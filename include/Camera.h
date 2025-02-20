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
  const glm::vec3 &GetFront() const { return mFront; }
  const bool &GetStatic() const { return mStatic; }
  const glm::mat4 &GetView() const { return mView; }
  const std::string &GetBgImage() const { return mBgImage; }

  void SetFront(const glm::vec3 &front) { mFront = front; }
  void SetPosition(const glm::vec3 &position) { mPosition = position; }

  void SetTransformation(const glm::vec3 &translation,
                         const glm::mat3 &rotation);
  void SetStatic(bool stati) { mStatic = stati; }

  void SetParameters(const CameraParameters &params);

private:
  glm::ivec2 mResolution;
  float mFOV;
  glm::vec3 mPosition;
  glm::vec3 mFront;
  glm::vec3 mUp;
  float mAspectRatio;
  glm::mat4 mMatrix;
  glm::mat4 mView;

  float mMoveSpeed = 3;
  float mRotateSpeed = 0.5f;
  glm::vec2 mLastMousePosition;

  bool mStatic = false;

  std::string mBgImage = "";
};
