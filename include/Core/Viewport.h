#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <imgui.h>
#include <memory>
#include <queue>
#include <vector>

#include "Core/Camera/Camera.h"
#include "Core/Generator.h"
#include "Core/IAppMode.h"
#include "Core/Loader/ExampleLoader.h"

#include "Rendering/Buffers/FBO.h"
#include "Rendering/Models/Quad.h"
#include "Rendering/Shaders/Renderer.h"

#include "Managers/CameraManager.h"
#include "Managers/ModelManager.h"
#include "Managers/PhysicsManager.h"
#include "Managers/TextureManager.h"

class Viewport : public IAppMode {
public:
  Viewport(BaseFolders *folders);

  // IAppMode interface
  void OnActivate() override;
  void OnDeactivate() override;
  void Update() override;
  void Render() override;
  void RenderUI() override;

  void SetTextureManager(TextureManager *tm) { mTextureManager = tm; }
  void SetExampleLoader(ExampleLoader *el) { mExampleLoader = el; }

private:
  void handleMain();
  void updateMain(const ImVec2 &windowSize);
  void renderMain();

  void handleSettings();
  void handleDebug();

  void handleResolutionChange();

  void handleOpenParams();
  void handleOpenModel();

  void addModel(const std::string &modelPath);
  void removeModel();

  void removeCamera();
  void switchCamFBO();

  void handleLoad();
  void loadExample();

private:
  BaseFolders *mBaseFolders = nullptr;
  ExampleLoader *mExampleLoader = nullptr;

  std::unique_ptr<Renderer> mRenderer;
  std::unique_ptr<ModelManager> mModelManager;
  std::unique_ptr<CameraManager> mCameraManager;
  std::unique_ptr<Quad> mBgQuad;
  std::unique_ptr<Quad> mDimQuad;
  std::unique_ptr<ViewMode> mViewMode;
  std::unique_ptr<Generator> mGenerator;
  std::unique_ptr<PhysicsManager> mPhysicsManager;

  ImVec2 mImageOffset;
  glm::vec2 mImageSize = glm::vec2(0);

  Camera *mCamera = nullptr;
  FBO *mFrameBuffer = nullptr;

  TextureManager *mTextureManager = nullptr;
  Texture *mBgTexture = nullptr;

  float mMaxDim = 0.0f;
  float mDim = 0.0f;

  std::vector<std::string> mResolutionNames = {"480p", "720p", "1080p", "1440p",
                                               "4K"};
  std::vector<int> mResolutionHeights = {480, 720, 1080, 1440, 2160};
  int mCurrentResolution = 0;

  std::queue<std::string> mModelLoadingQueue;
  std::queue<std::string> mCameraLoadingQueue;
};
