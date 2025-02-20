#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <imgui.h>
#include <memory>
#include <vector>

#include "Camera.h"
#include "FBO.h"
#include "Mesh.h"
#include "Quad.h"
#include "Shader.h"

#include "CameraParameters.h"
#include "CameraParametersManager.h"
#include "FileSystem.h"

#include "CameraManager.h"
#include "TextureManager.h"
#include "PhysicsManager.h"
#include "RenderManager.h"
#include "MeshManager.h"

class Viewport {
public:
  Viewport();
  void SetWindow(GLFWwindow *window) { mWindow = window; }

  void Update(float ts);

private:
  void renderSceneToFramebuffer();
  void handleOpenParams();

  void addCamera(const std::string &path);
  void removeCamera();
  void switchCamFBO();

private:
  GLFWwindow *mWindow;

  std::unique_ptr<Shader> mShadedShader;
  std::unique_ptr<Shader> mFlatShader;
  std::unique_ptr<Shader> mSegmentedShader;
  std::unique_ptr<Shader> mBgQuadShader;

  MeshManager mMeshManager;

  CameraManager mCameraManager;
  std::shared_ptr<Camera> mCamera;
  std::shared_ptr<FBO> mFrameBuffer;

  std::string mFolderPath = "/home/tilen/dev/Omvex/test1/1.jpg";

  CameraParametersManager mCameraParametersManager;
  std::unique_ptr<CameraParameters> mCameraParameters = nullptr;

  TextureManager mTextureManager;
  std::shared_ptr<Texture> mBgTexture;
  std::unique_ptr<Quad> mBgQuad;

  ViewMode mViewMode = ViewMode::Shaded;

  PhysicsManager mPhysicsManager;
  RenderManager mRenderManager;
};
