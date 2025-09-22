#include "Core/Viewport.h"

#include "Utilities/FileSystem.h"
#include "Utilities/ImGuiHelpers.h"
#include "Utilities/Random.h"

Viewport::Viewport(BaseFolders *folders) {
  mBaseFolders = folders;
  mViewMode = std::make_unique<ViewMode>(ViewMode::Color);
  mBgQuad = std::make_unique<Quad>();
  mDimQuad = std::make_unique<Quad>();
  mModelManager = std::make_unique<ModelManager>();
  mPhysicsManager = std::make_unique<PhysicsManager>();
  mCameraManager = std::make_unique<CameraManager>();
  mRenderer = std::make_unique<Renderer>(mBaseFolders->Shaders);
  mGenerator =
      std::make_unique<Generator>(mCameraManager.get(), mModelManager.get(),
                                  mPhysicsManager.get(), mViewMode.get());
  Logger::Success("Viewport initialized");
}

void Viewport::OnActivate() { Logger::Info("Viewport activated"); }
void Viewport::OnDeactivate() { Logger::Info("Viewport deactivated"); }
void Viewport::RenderUI() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Settings")) {
      if (ImGui::BeginMenu("Resolution")) {
        handleResolutionChange();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::BeginMenu("Camera")) {
        if (ImGui::MenuItem("Add")) handleOpenParams();
        if (ImGui::MenuItem("Remove")) removeCamera();
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Model")) {
        if (ImGui::MenuItem("Add")) handleOpenModel();
        if (ImGui::MenuItem("Remove")) removeModel();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Example")) loadExample();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void Viewport::handleMain() {
  ImGui::Begin("Main");
  ImVec2 windowSize = ImGui::GetContentRegionAvail();
  updateMain(windowSize);
  renderMain();
  ImGui::End();
}
void Viewport::updateMain(const ImVec2 &windowSize) {
  if (!mFrameBuffer || !mCamera) return;

  float aspectRatio = mCamera->GetAspectRatio();
  ImGuiHelpers::RecalculateImageSizeOffset(windowSize, aspectRatio, mImageSize,
                                           mImageOffset);
}
void Viewport::renderMain() {
  if (mFrameBuffer) {
    ImGui::SetCursorPos(mImageOffset);
    mFrameBuffer->ColorTexture->Bind();
    ImGui::Image(
        (ImTextureID)(intptr_t)mFrameBuffer->ColorTexture->GetTextureID(),
        ImVec2(mImageSize.x, mImageSize.y));
    mFrameBuffer->ColorTexture->Unbind();
  } else {
    ImGuiHelpers::CenterText("You need to add camera");
  }
}

void Viewport::handleSettings() {
  ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize);
  ImGui::Text("VIEWPORT");

  ImGui::Text("View Mode:");
  ImGui::SameLine();
  if (ImGui::RadioButton("Color", *mViewMode == ViewMode::Color)) {
    *mViewMode = ViewMode::Color;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Segmentation",
                         *mViewMode == ViewMode::Segmentation)) {
    *mViewMode = ViewMode::Segmentation;
  }

  mCameraManager->ShowCameras();
  if (ImGui::Button("Add Camera")) handleOpenParams();
  ImGui::SameLine();
  if (ImGui::Button("Remove Camera")) removeCamera();

  mModelManager->ShowModels();
  if (ImGui::Button("Add Model")) handleOpenModel();
  ImGui::SameLine();
  if (ImGui::Button("Remove Model")) removeModel();
  ImGui::Separator();

  ImGui::Text("Simulation");
  ImGui::DragFloat("SpawningArea", &mPhysicsManager->ModSpawningSpace());
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("This slider controls spawning area half length.");
  }

  if (ImGui::Button("Simulate") && mPhysicsManager->GetBodyCount() > 0)
    mPhysicsManager->Simulate();
  ImGui::Separator();
  ImGui::Text("Renderer");
  ImGui::SliderFloat("MaxDim", &mMaxDim, 0.0f, 1.0f);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("This slider controls the maximum dimming effect on the "
                      "image. Higher values make the image darker.");
  }
  ImGui::InputInt("Renders", &mGenerator->ModifyNumRenders());
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("This input sets number of images to be rendered");
  }
  if (!mGenerator->IsRunning()) {
    if (ImGui::Button("Start Render") && mCameraManager->GetCount() > 0) {
      std::string folder = FileSystem::SelectFolder(mBaseFolders->Active);
      mGenerator->Start(folder);
    }
  }
  if (mGenerator->IsRunning()) {
    if (ImGui::Button("Stop Render")) mGenerator->Stop();
  }
  if (mGenerator->IsRunning()) ImGui::ProgressBar(mGenerator->GetProgress());
  ImGui::End();
}

void Viewport::handleDebug() {
  ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoResize);
  ImGui::Text("Active Folder: %s", mBaseFolders->Active.c_str());
  ImGui::Text("mDim: %f", mDim);
  ImGui::Text("mImageResolutionHeight: %i",
              mResolutionHeights[mCurrentResolution]);
  ImGui::Separator();
  ImGui::Text("ModelManager");
  ImGui::Text(" -Count: %i", mModelManager->GetCount());
  ImGui::Text(" -SelectedID: %i", mModelManager->GetSelectedId());
  ImGui::Separator();
  ImGui::Text("PhysicsManager: ");
  ImGui::Text(" -Count: %i", mPhysicsManager->GetBodyCount());
  ImGui::Text(" -IsSimulating: %i", mPhysicsManager->IsSimulating());
  ImGui::Separator();
  ImGui::Text("CameraManager: ");
  ImGui::Text(" -Count: %i", mCameraManager->GetCount());
  ImGui::Text(" -SelectedID: %i", mCameraManager->GetSelectedId());
  if (mCamera) {
    ImGui::Text("Camera:");
    ImGui::Text(" -Resolution: %s: %i, %i",
                mResolutionNames[mCurrentResolution].c_str(),
                mCamera->GetResolution().x, mCamera->GetResolution().y);
    ImGui::Text(" -BgImage: %s", mCamera->GetBgImage().c_str());
  }

  ImGui::End();
}

void Viewport::Render() {
  mRenderer->Begin(mCamera, mFrameBuffer, mViewMode.get(), mBgQuad.get());

  for (size_t i = 0; i < mModelManager->GetCount(); i++) {
    Model *model = mModelManager->GetModel(i);
    glm::vec3 color = mModelManager->GetSegmentedColors()[i];
    mRenderer->RenderModel(mCamera, mFrameBuffer, mViewMode.get(), model,
                           color);
  }
  mRenderer->End(mDimQuad.get(), mDim, mViewMode.get(), mFrameBuffer);
}

void Viewport::Update() {
  handleLoad();
  handleMain();
  handleSettings();
  handleDebug();
  Logger::ShowLogs();

  if (mGenerator->IsRunning() && !mPhysicsManager->IsSimulating() &&
      mGenerator->NeedSim()) {
    mPhysicsManager->Simulate();
  }
  if (!mGenerator->IsRunning()) mDim = mMaxDim;
  mPhysicsManager->Update(mModelManager->GetModels());
  if (mGenerator->IsRunning() && !mPhysicsManager->IsSimulating())
    mDim = Random::Float(0.0f, mMaxDim);
  if (!mPhysicsManager->IsSimulating()) {
    mGenerator->Update();
  }

  if (mCameraManager->HandleSwitching()) {
    switchCamFBO();
  }
}

void Viewport::handleResolutionChange() {
  for (size_t i = 0; i < mResolutionNames.size(); i++) {
    if (ImGui::MenuItem(mResolutionNames[i].c_str(), NULL,
                        mCurrentResolution == i)) {
      mCurrentResolution = static_cast<int>(i);
      mCameraManager->ChangeResolution(mResolutionHeights[mCurrentResolution]);
      Logger::Debug("Resolution changed to: " +
                    mResolutionNames[mCurrentResolution]);
    }
  }
}

void Viewport::removeCamera() {
  mCameraManager->Remove(mCameraManager->GetSelectedId());
  switchCamFBO();
}

void Viewport::switchCamFBO() {
  mCamera = mCameraManager->GetCamera();
  mFrameBuffer = mCameraManager->GetFBO();
  if (!mCamera || !mFrameBuffer) return;

  mBgTexture = mTextureManager->GetTexture(mCamera->GetBgImage());
  mBgQuad->SetTexture(mBgTexture);
  Logger::Debug("Switch to camera " + mCamera->GetBgImage());
}

void Viewport::handleOpenParams() {
  const std::vector<std::string> &paramFilePaths =
      FileSystem::OpenFiles(mBaseFolders->Active, "Parameters .json", "*.json");
  for (const std::string &paramFilePath : paramFilePaths) {
    if (!paramFilePath.empty()) mCameraLoadingQueue.push(paramFilePath);
  }
}

// MODELS
void Viewport::handleOpenModel() {
  const std::vector<std::string> &modelFilePaths =
      FileSystem::OpenFiles(mBaseFolders->Active, "Model .obj", "*.obj");
  for (const std::string &modelFilePath : modelFilePaths) {
    if (!modelFilePath.empty()) mModelLoadingQueue.push(modelFilePath);
  }
}
void Viewport::addModel(const std::string &modelPath) {
  std::unique_ptr<Model> model =
      std::make_unique<Model>(modelPath, mTextureManager);
  mPhysicsManager->AddModel(model.get());
  mModelManager->AddModel(std::move(model));
}
void Viewport::removeModel() {
  int id = mModelManager->GetSelectedId();
  mPhysicsManager->RemoveBody(id);
  mModelManager->Remove(id);
}

void Viewport::handleLoad() {
  if (!mCameraLoadingQueue.empty()) {
    const std::string cameraPath = mCameraLoadingQueue.front();
    mCameraLoadingQueue.pop();
    mCameraManager->AddCameraFBO(cameraPath,
                                 mResolutionHeights[mCurrentResolution]);
    return;
  }
  if (!mModelLoadingQueue.empty()) {
    const std::string modelPath = mModelLoadingQueue.front();
    mModelLoadingQueue.pop();
    addModel(modelPath);
  }
}
void Viewport::loadExample() {
  mExampleLoader->LoadCameras(mCameraLoadingQueue);
  mExampleLoader->LoadModels(mModelLoadingQueue);
}
