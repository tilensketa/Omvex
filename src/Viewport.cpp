#include "Viewport.h"
#include "Random.h"
#include "Utils.h"

Viewport::Viewport(const std::string &baseFolder) {
  mShaderFolderPath = baseFolder + "/shaders/";
  mExampleFolderPath = baseFolder + "/example/";
  Logger::getInstance().Debug("Shader path: " + mShaderFolderPath);
  Logger::getInstance().Debug("Example path: " + mExampleFolderPath);

  mCameraParameters = std::make_unique<CameraParameters>("");

  mBgQuad = std::make_unique<Quad>();

  mSegmentedShader = std::make_unique<Shader>(
      mShaderFolderPath, "segmentedVert.glsl", "segmentedFrag.glsl");
  mShadedShader = std::make_unique<Shader>(mShaderFolderPath, "shadedVert.glsl",
                                           "shadedFrag.glsl");
  mBgQuadShader = std::make_unique<Shader>(mShaderFolderPath, "quadVert.glsl",
                                           "quadFrag.glsl");

  if (mBgTexture)
    mBgQuad->SetTexture(mBgTexture->GetTextureID());

  mSpawningPreviewMesh = std::make_unique<Mesh>(
      Mesh::CreateQuad(mPhysicsManager.GetSpawningSpace()));

  mRenderManager.SetViewMode(&mViewMode);
  mRenderManager.SetCameraManager(&mCameraManager);

  glEnable(GL_DEPTH_TEST);

  glm::mat4 mat = glm::mat4(1);
  mat[2][2] = -1;
  mShadedShader->Activate();
  mShadedShader->SetMat4("matrix", mat);
  mSegmentedShader->Activate();
  mSegmentedShader->SetMat4("matrix", mat);
}

void Viewport::Update(float ts) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Settings")) {
      if (ImGui::BeginMenu("Resolution")) {
        for (size_t i = 0; i < mResolutionNames.size(); i++) {
          if (ImGui::MenuItem(mResolutionNames[i].c_str(), NULL,
                              mCurrentResolution == i)) {
            mCurrentResolution = static_cast<int>(i);
            mImageResolutionHeight = mResolutionHeights[i];
            Logger::getInstance().Debug("Resolution changed to: " +
                                        mResolutionNames[mCurrentResolution]);
            for (size_t j = 0; j < mCameraManager.GetCount(); j++) {
              std::shared_ptr<FBO> &fbo = mCameraManager.GetFBOs()[j];
              std::shared_ptr<Camera> &camera = mCameraManager.GetCameras()[j];
              glm::ivec2 resolution = mCameraParameters->ImageCalibratedSize;
              float aspectRatio = float(resolution.x) / resolution.y;
              int width =
                  static_cast<int>(mImageResolutionHeight * aspectRatio);
              fbo->Resize(width, mImageResolutionHeight);
              camera->SetResolution(glm::ivec2(width, mImageResolutionHeight));
            }
          }
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::BeginMenu("Camera")) {
        if (ImGui::MenuItem("Add")) {
          handleOpenParams();
        } else if (ImGui::MenuItem("Remove")) {
          removeCamera();
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Model")) {
        if (ImGui::MenuItem("Add")) {
          handleOpenModel();
        } else if (ImGui::MenuItem("Remove")) {
          removeModel();
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Example")) {
        loadExample();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (mRenderManager.IsRendering() && !mPhysicsManager.IsSimulating() &&
      (mRenderManager.GetRenderId() % mCameraManager.GetCount() == 0 ||
       mRenderManager.GetRenderId() == 0) &&
      mRenderManager.GetRenderSubId() == 0) {
    mPhysicsManager.SetSimulating(true);
    mPhysicsManager.SetSimulationFrame(0);
  }
  mPhysicsManager.Update(mModelManager.GetModels());

  bool switchCam = mCameraManager.HandleSwitching(mWindow);
  if (switchCam) {
    switchCamFBO();
  }
  renderSceneToFramebuffer();

  // Render if simulation is over
  bool canRender = !mPhysicsManager.IsSimulating();
  mRenderManager.Update(mFrameBuffer, canRender);

  // ImGui window with the framebuffer
  ImGui::Begin("MainView");
  if (mFrameBuffer == nullptr) {
    std::string text = "You need to add camera!";
    Utils::ImGuiHelpers::CenterText(text);
  } else {
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    float aspectRatio = mCamera->GetAspectRatio();
    float newWidth = windowSize.x;
    float newHeight = windowSize.x / aspectRatio;
    if (newHeight > windowSize.y) {
      newHeight = windowSize.y;
      newWidth = windowSize.y * aspectRatio;
    }
    ImVec2 imageSize = ImVec2(newWidth, newHeight);
    ImVec2 offset = ImVec2((windowSize.x - imageSize.x) * 0.5f,
                           (windowSize.y - imageSize.y) * 0.5f);

    ImGui::SetCursorPos(offset);
    ImGui::Image(
        (ImTextureID)(intptr_t)mFrameBuffer->colorTexture->GetTextureID(),
        imageSize);
  }

  ImGui::End();

  ImGui::Begin("Settings");
  ImGui::Text("VIEWPORT");

  ImGui::Text("View Mode:");
  ImGui::SameLine();
  if (ImGui::RadioButton("Flat", mViewMode == ViewMode::Flat)) {
    mViewMode = ViewMode::Flat;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Shaded", mViewMode == ViewMode::Shaded)) {
    mViewMode = ViewMode::Shaded;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Segmented2D", mViewMode == ViewMode::Segmented2D)) {
    mViewMode = ViewMode::Segmented2D;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Segmented3D", mViewMode == ViewMode::Segmented3D)) {
    mViewMode = ViewMode::Segmented3D;
  }

  mCameraManager.ShowCameras();
  if (ImGui::Button("Add Camera")) {
    handleOpenParams();
  }
  ImGui::SameLine();
  if (ImGui::Button("Remove Camera")) {
    removeCamera();
  }

  mModelManager.ShowModels();
  if (ImGui::Button("Add Model")) {
    handleOpenModel();
  }
  ImGui::SameLine();
  if (ImGui::Button("Remove Model")) {
    removeModel();
  }
  ImGui::Separator();
  ImGui::Text("Simulation");
  if (ImGui::SliderFloat("SpawningArea", &mPhysicsManager.ModSpawningSpace(),
                         0.0f, 100.0f)) {
    mSpawningPreviewMesh = std::make_unique<Mesh>(
        Mesh::CreateQuad(mPhysicsManager.GetSpawningSpace()));
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip(
        "This slider controls spawning area half length (yellow quad).");
  }

  if (ImGui::Button("Simulate")) {
    if (mPhysicsManager.GetBodyCount() == 1) {
      if (!ImGui::IsPopupOpen("ErrorPhysics")) {
        ImGui::OpenPopup("ErrorPhysics");
      }
    } else {
      mPhysicsManager.SetSimulating(true);
      mPhysicsManager.SetSimulationFrame(0);
    }
  }
  if (ImGui::BeginPopup("ErrorPhysics")) {
    ImGui::Text("You need to have a model in the scene");
    ImGui::EndPopup();
  }
  ImGui::Separator();
  ImGui::Text("Renderer");
  ImGui::SliderFloat("MaxDim", &mMaxDim, 0.0f, 1.0f);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("This slider controls the maximum dimming effect on the "
                      "image. Higher values make the image darker.");
  }
  ImGui::InputInt("Renders", &mRenderManager.ModRenders());
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("This input sets number of images to be rendered");
  }
  if (ImGui::Button("Render")) {
    if (mCameraManager.GetCount() == 0) {
      if (!ImGui::IsPopupOpen("ErrorRender")) {
        ImGui::OpenPopup("ErrorRender");
      }
    } else {
      mRenderManager.UpdateRenderClick(*mActiveFolder,
                                       mModelManager.GetSegmentedColors(),
                                       mModelManager.GetModelNames());
    }
  }
  if (mRenderManager.IsRendering()) {
    ImGui::SameLine();
    if (ImGui::Button("Stop Rendering")) {
      mRenderManager.StopRendering();
    }
  }
  if (ImGui::BeginPopup("ErrorRender")) {
    ImGui::Text("You need to have a camera in the scene");
    ImGui::EndPopup();
  }
  if (mRenderManager.IsRendering()) {
    ImGui::ProgressBar(float(mRenderManager.GetRenderId()) /
                       (mRenderManager.GetRenders()));
  }
  ImGui::End();

  ImGui::Begin("Debug");
  ImGui::Text("Active Folder: %s", mActiveFolder->c_str());
  ImGui::Text("mDim: %f", mDim);
  ImGui::Text("mImageResolutionHeight: %i", mImageResolutionHeight);
  ImGui::Separator();
  ImGui::Text("ModelManager");
  ImGui::Text(" -Count: %i", mModelManager.GetCount());
  ImGui::Text(" -SelectedID: %i", mModelManager.GetSelectedId());
  ImGui::Text(" -sModelId: %i", mModelManager.sModelId);
  ImGui::Separator();
  ImGui::Text("PhysicsManager: ");
  ImGui::Text(" -Count: %i", mPhysicsManager.GetBodyCount());
  ImGui::Text(" -IsSimulating: %i", mPhysicsManager.IsSimulating());
  ImGui::Separator();
  ImGui::Text("CameraManager: ");
  ImGui::Text(" -Count: %i", mCameraManager.GetCount());
  ImGui::Text(" -SelectedID: %i", mCameraManager.GetSelectedId());
  if (mCamera != nullptr) {
    ImGui::Text("Camera:");
    ImGui::Text(" -Resolution: %s: %i, %i",
                mResolutionNames[mCurrentResolution].c_str(),
                mCamera->GetResolution().x, mCamera->GetResolution().y);
    ImGui::Text(" -BgImage: %s", mCamera->GetBgImage().c_str());
  }

  ImGui::Text("Segmented Colors:");
  for (size_t i = 0; i < mModelManager.GetCount(); i++) {
    ImGui::ColorEdit3(std::to_string(i).c_str(),
                      &mModelManager.GetSegmentedColors()[i].x);
  }
  ImGui::End();
  Logger::getInstance().ShowLogs();
}

void Viewport::renderSceneToFramebuffer() {
  if (mCamera == nullptr || mFrameBuffer == nullptr) {
    return;
  }

  mFrameBuffer->Bind();
  glViewport(0, 0, mCamera->GetResolution().x, mCamera->GetResolution().y);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mCamera->UpdateMatrix();

  if (mRenderManager.IsRendering() && !mPhysicsManager.IsSimulating()) {
    mDim = Random::Float(0.0f, mMaxDim);
  } else {
    mDim = mMaxDim;
  }
  // Draw background if not segmented
  if (mViewMode != ViewMode::Segmented2D &&
      mViewMode != ViewMode::Segmented3D) {
    if (mCamera->GetBgImage() != "") {
      glDisable(GL_DEPTH_TEST);
      mBgQuadShader->Activate();
      mBgQuadShader->SetFloat("RandomDim", mDim);
      mBgQuad->Draw(*mBgQuadShader, true);
      glEnable(GL_DEPTH_TEST);
    }
  }
  // Draw spawning area mesh
  if (!mRenderManager.IsRendering() && mViewMode != ViewMode::Segmented2D &&
      mViewMode != ViewMode::Segmented3D) {
    mSegmentedShader->Activate();
    mSegmentedShader->SetVec3("uniqueColor", glm::vec3(1, 1, 0));
    mSpawningPreviewMesh->Draw(*mSegmentedShader, *mCamera, false);
  }

  for (size_t i = 0; i < mModelManager.GetCount(); i++) {
    std::shared_ptr<Model> &model = mModelManager.GetModels()[i];
    if (mViewMode == ViewMode::Segmented3D) {
      int j = 0;
      for (Mesh &corner : model->GetFeautureMeshes()) {
        mSegmentedShader->Activate();
        glm::vec3 &uniqueColor = mModelManager.GetSegmentedColors()[i * 12 + j];
        mSegmentedShader->SetVec3("uniqueColor", uniqueColor);
        corner.Draw(*mSegmentedShader, *mCamera, true);
        j++;
      }
    }
    for (Mesh &mesh : model->GetMeshes()) {
      if (mViewMode == ViewMode::Flat) {
        mShadedShader->Activate();
        mShadedShader->SetBool("IsShaded", false);
        mShadedShader->SetFloat("RandomDim", mDim);
        mesh.Draw(*mShadedShader, *mCamera, true);
      } else if (mViewMode == ViewMode::Shaded) {
        mShadedShader->Activate();
        mShadedShader->SetBool("IsShaded", true);
        mShadedShader->SetFloat("RandomDim", mDim);
        mesh.Draw(*mShadedShader, *mCamera, true);
      } else if (mViewMode == ViewMode::Segmented2D) {
        mSegmentedShader->Activate();
        glm::vec3 &uniqueColor = mModelManager.GetSegmentedColors()[i * 12];
        mSegmentedShader->SetVec3("uniqueColor", uniqueColor);
        mesh.Draw(*mSegmentedShader, *mCamera, true);
      }
    }
  }
  mFrameBuffer->Unbind();
}

void Viewport::handleOpenParams() {
  const std::vector<std::string> &paramFilePaths =
      FileSystem::OpenFiles(*mActiveFolder, "Parameters .json", "*.json");
  for (const std::string &paramFilePath : paramFilePaths) {
    if (paramFilePath != "")
      addCamera(paramFilePath);
  }
}

void Viewport::addCamera(const std::string &path) {
  mCameraParameters->Load(path);
  std::string folderPath = FileSystem::GetDirectoryFromPath(path);
  glm::ivec2 resolution = mCameraParameters->ImageCalibratedSize;
  float aspectRatio = float(resolution.x) / resolution.y;
  int width = static_cast<int>(mImageResolutionHeight * aspectRatio);
  mCamera =
      std::make_shared<Camera>(width, mImageResolutionHeight, glm::vec3(0));
  mCamera->SetParameters(folderPath, *mCameraParameters);
  mCameraManager.AddCamera(mCamera);
  std::string texturePath =
      folderPath + "/" + mCameraParameters->RefImageFilePath;
  mBgTexture = mTextureManager->getTexture(texturePath);
  mBgQuad->SetTexture(mBgTexture->GetTextureID());
  mFrameBuffer = std::make_shared<FBO>(width, mImageResolutionHeight);
  mCameraManager.AddFBO(mFrameBuffer);
}

void Viewport::removeCamera() {
  bool result = mCameraManager.Remove();
  switchCamFBO();
}

void Viewport::switchCamFBO() {
  mCamera = mCameraManager.GetCamera();
  if (mCamera == nullptr) {
    mFrameBuffer = nullptr;
    return;
  }
  if (mCamera->GetBgImage() != "") {
    mBgTexture = mTextureManager->getTexture(mCamera->GetBgImage());
    mBgQuad->SetTexture(mBgTexture->GetTextureID());
    mFrameBuffer = mCameraManager.GetFBO();
    Logger::getInstance().Debug("Switch to camera " +
                                std::to_string(mCameraManager.GetSelectedId()));
  }
}

void Viewport::handleOpenModel() {
  const std::vector<std::string> &modelFilePaths =
      FileSystem::OpenFiles(*mActiveFolder, "Model .obj", "*.obj");
  for (const std::string &modelFilePath : modelFilePaths) {
    if (modelFilePath != "") {
      addModel(modelFilePath);
    }
  }
}

void Viewport::addModel(const std::string &modelPath) {
  std::shared_ptr<Model> model = std::make_shared<Model>(modelPath);
  mPhysicsManager.AddModel(model);
  mModelManager.AddModel(model);
}

void Viewport::removeModel() {
  mPhysicsManager.RemoveBody(mModelManager.GetSelectedId());
  mModelManager.Remove();
}

void Viewport::loadExample() {
  for (size_t i = 1; i < 29; i++) {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << i;
    std::string filename = oss.str();
    std::string cameraPath =
        mExampleFolderPath + "backgrounds/" + filename + ".json";
    addCamera(cameraPath);
  }
  std::vector<std::string> modelFiles = {
      "Deodorant/deodorant.obj",
      "Hammer/hammer.obj",
      "Light/light.obj",
      "Meter5m/meter5m.obj",
      "Multimeter/multimeter.obj",
      "Power_distributor/power_distributor.obj",
      "Rubiks_cube/rubiks_cube.obj",
      "Sliding_scale/sliding_scale.obj",
      "Uhu/uhu.obj",
      "Wrench/wrench.obj"};
  for (const std::string &modelFile : modelFiles) {
    std::string modelPath = mExampleFolderPath + "models/" + modelFile;
    addModel(modelPath);
  }
}
