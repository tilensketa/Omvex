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
  Viewport(const std::string &baseFolder);
  void SetWindow(GLFWwindow *window) { mWindow = window; }
  void SetActiveFolder(std::shared_ptr<std::string> &activeFolder) {
    mActiveFolder = activeFolder;
  }
  void SetTextureManager(std::shared_ptr<TextureManager> &textureManager) {
    mTextureManager = textureManager;
  }

  void Update(float ts);

private:
  void renderSceneToFramebuffer();
  void handleOpenParams();
  void handleOpenModel();

  void addModel(const std::string &modelPath);
  void removeModel();

  void addCamera(const std::string &path);
  void removeCamera();
  void switchCamFBO();

  void loadExample();

private:
  GLFWwindow *mWindow;

  std::shared_ptr<std::string> mActiveFolder = nullptr;
  std::string mShaderFolderPath = "";
  std::string mExampleFolderPath = "";

  std::unique_ptr<Shader> mShadedShader;
  std::unique_ptr<Shader> mSegmentedShader;
  std::unique_ptr<Shader> mBgQuadShader;

  ModelManager mModelManager;
  std::unique_ptr<Mesh> mSpawningPreviewMesh;

  CameraManager mCameraManager;
  std::shared_ptr<Camera> mCamera;
  std::shared_ptr<FBO> mFrameBuffer;

  std::unique_ptr<CameraParameters> mCameraParameters = nullptr;

  std::shared_ptr<TextureManager> mTextureManager;
  std::shared_ptr<Texture> mBgTexture;
  std::unique_ptr<Quad> mBgQuad;

  ViewMode mViewMode = ViewMode::Shaded;
  float mMaxDim = 0.0f;
  float mDim = 0.0f;

  std::vector<std::string> mResolutionNames = {"480p", "720p", "1080p", "1440p",
                                               "4K"};
  std::vector<int> mResolutionHeights = {480, 720, 1080, 1440, 2160};
  int mCurrentResolution = 2;
  int mImageResolutionHeight = mResolutionHeights[mCurrentResolution];

  PhysicsManager mPhysicsManager;
  RenderManager mRenderManager;
};
