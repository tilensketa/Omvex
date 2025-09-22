#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Core/Camera/CameraParameters.h"

class Camera {
public:
  Camera(int width, int height, glm::vec3 position);

  const glm::vec3 &GetPosition() const { return mPosition; }
  const float &GetAspectRatio() const { return mAspectRatio; }
  const glm::ivec2 &GetResolution() const { return mResolution; }
  const std::string &GetBgImage() const { return mBgImage; }
  const glm::mat4 &GetMatrix() const { return mMatrix; }

  void SetResolution(const glm::ivec2 &res);

  void SetParameters(const std::string &folderPath,
                     const CameraParameters &params);
  CameraParameters *GetParameters() const { return mParameters.get(); }

private:
  glm::ivec2 mResolution;
  float mAspectRatio;
  float mNearPlane = 0.1f;
  float mFarPlane = 1000.0f;

  glm::vec3 mPosition;
  glm::vec3 mTarget;
  glm::vec3 mUp;

  glm::mat4 mView;
  glm::mat4 mProjection;
  glm::mat4 mMatrix;

  std::string mBgImage = "";
  std::unique_ptr<CameraParameters> mParameters;
};
