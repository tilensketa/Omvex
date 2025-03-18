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
#include "FileSystem.h"

#include "CameraManager.h"
#include "Model.h"
#include "ModelManager.h"
#include "PhysicsManager.h"
#include "RenderManager.h"
#include "TextureManager.h"

class Viewport {
public:
  Viewport();
  void SetWindow(GLFWwindow *window) { mWindow = window; }
  void SetActiveFolder(std::shared_ptr<std::string> &activeFolder) {
    mActiveFolder = activeFolder;
  }

  void Update(float ts);

private:
  void getRelativePaths();
  void renderSceneToFramebuffer();
  void handleOpenParams();
  void handleOpenModel();

  void addModel(const std::string &modelPath);
  void removeModel();

  void addCamera(const std::string &path);
  void removeCamera();
  void switchCamFBO();

private:
  GLFWwindow *mWindow;

  std::string mShaderFolderPath = "";
  std::string mModelFolderPath = "";

  std::unique_ptr<Shader> mShadedShader;
  std::unique_ptr<Shader> mSegmentedShader;
  std::unique_ptr<Shader> mBgQuadShader;

  ModelManager mModelManager;

  CameraManager mCameraManager;
  std::shared_ptr<Camera> mCamera;
  std::shared_ptr<FBO> mFrameBuffer;

  std::shared_ptr<std::string> mActiveFolder = nullptr;

  std::unique_ptr<CameraParameters> mCameraParameters = nullptr;

  TextureManager mTextureManager;
  std::shared_ptr<Texture> mBgTexture;
  std::unique_ptr<Quad> mBgQuad;

  ViewMode mViewMode = ViewMode::Shaded;

  PhysicsManager mPhysicsManager;
  RenderManager mRenderManager;
};
