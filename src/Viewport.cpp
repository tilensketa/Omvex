#include "Viewport.h"
#include "Random.h"

#include "Colors.h"
#include "Utils.h"

#define SHADER_PATH "../shaders/"
#define MODEL_PATH "../models/"

Viewport::Viewport() {
  mCameraParameters = std::make_unique<CameraParameters>("");

  mBgQuad = std::make_unique<Quad>();

  // Load all backgrounds
  for (int i = 1; i < 7; i++) {
    std::string path =
        "/home/tilen/dev/Omvex/test1/" + std::to_string(i) + ".json";
    addCamera(path);
  }

  mSegmentedShader = std::make_unique<Shader>(SHADER_PATH "segmentedVert.glsl",
                                              SHADER_PATH "segmentedFrag.glsl");
  mShadedShader = std::make_unique<Shader>(SHADER_PATH "shadedVert.glsl",
                                           SHADER_PATH "shadedFrag.glsl");
  mBgQuadShader = std::make_unique<Shader>(SHADER_PATH "quadVert.glsl",
                                           SHADER_PATH "quadFrag.glsl");

  if (mBgTexture)
    mBgQuad->SetTexture(mBgTexture->GetTextureID());

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

  // First ImGui window with the framebuffer
  ImGui::Begin("MainView");
  if (mFrameBuffer == nullptr) {
    ImGui::Text("You need to add camera!");
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
  if (ImGui::RadioButton("Flat", mViewMode == ViewMode::Flat)) {
    mViewMode = ViewMode::Flat;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Shaded", mViewMode == ViewMode::Shaded)) {
    mViewMode = ViewMode::Shaded;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Segmented", mViewMode == ViewMode::Segmented)) {
    mViewMode = ViewMode::Segmented;
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
  if (ImGui::Button("Add Multimeter")) {
    addModel(MODEL_PATH "Multimeter/multimeter.obj");
  }
  ImGui::SameLine();
  if (ImGui::Button("Add Power Dist")) {
    addModel(MODEL_PATH "Power_distributor/power_distributor.obj");
  }
  ImGui::SameLine();
  if (ImGui::Button("Add Sliding Scale")) {
    addModel(MODEL_PATH "Sliding_scale/sliding_scale.obj");
  }

  if (ImGui::Button("Add Model")) {
    handleOpenModel();
  }
  ImGui::SameLine();
  if (ImGui::Button("Remove Model")) {
    removeModel();
  }

  if (ImGui::Button("Simulate")) {
    mPhysicsManager.SetSimulating(true);
    mPhysicsManager.SetSimulationFrame(0);
  }
  ImGui::InputInt("Renders: ", &mRenderManager.ModRenders());
  if (ImGui::Button("Render")) {
    if (mCameraManager.GetCount() == 0) {
      ImGui::BeginPopup("Error");
      ImGui::Text("You need to have camera in scene");
      ImGui::EndPopup();
    } else {
      mRenderManager.UpdateRenderClick(mFolderPath);
    }
  }
  if (mRenderManager.IsRendering()) {
    ImGui::ProgressBar(float(mRenderManager.GetRenderId()) /
                       (mRenderManager.GetRenders()));
  }

  ImGui::Text("ModelManager SelectedID: %i", mModelManager.GetSelectedId());
  ImGui::Text("ModelManager Count: %i", mModelManager.GetCount());
  ImGui::Text("PhysicsManager Count: %i", mPhysicsManager.GetBodyCount());

  ImGui::Text("Segmented Colors:");
  for (int i = 0; i < mModelManager.GetCount(); i++) {
    ImGui::ColorEdit3(std::to_string(i).c_str(),
                      &mModelManager.GetSegmentedColors()[i].x);
  }
  ImGui::End();

  ImGui::Begin("Logger");
  std::vector<std::pair<LogLevel, std::string>> logs =
      Logger::getInstance().GetLogs();
  ImGui::PushTextWrapPos(ImGui::GetWindowWidth());
  for (const auto &log : logs) {
    ImU32 color = Colors::LogLevelToColor(static_cast<LogLevel>(log.first));
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::Text("%s", log.second.c_str());
    ImGui::PopStyleColor();
  }
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
    ImGui::SetScrollHereY(1.0f);
  }
  ImGui::PopTextWrapPos();
  ImGui::End();
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

  if (mViewMode != ViewMode::Segmented) {
    if (mCamera->GetBgImage() != "") {
      glDisable(GL_DEPTH_TEST);
      mBgQuad->Draw(*mBgQuadShader, true);
      glEnable(GL_DEPTH_TEST);
    }
  }

  for (int i = 0; i < mModelManager.GetCount(); i++) {
    std::shared_ptr<Model> &model = mModelManager.GetModels()[i];
    for (Mesh &mesh : model->GetMeshes()) {
      if (mViewMode == ViewMode::Flat) {
        mShadedShader->Activate();
        mShadedShader->SetBool("IsShaded", false);
        mesh.Draw(*mShadedShader, *mCamera, GL_TRIANGLES);
      } else if (mViewMode == ViewMode::Shaded) {
        mShadedShader->Activate();
        mShadedShader->SetBool("IsShaded", true);
        mesh.Draw(*mShadedShader, *mCamera, GL_TRIANGLES);
      } else if (mViewMode == ViewMode::Segmented) {
        mSegmentedShader->Activate();
        glm::vec3 &uniqueColor = mModelManager.GetSegmentedColors()[i];
        mSegmentedShader->SetVec3("uniqueColor", uniqueColor);
        mesh.Draw(*mSegmentedShader, *mCamera, GL_TRIANGLES);
      }
    }
  }
  mFrameBuffer->Unbind();
}

void Viewport::handleOpenParams() {
  const std::vector<std::string> &paramFilePaths =
      FileSystem::OpenFiles(mFolderPath, "Parameters .json", "*.json");
  for (const std::string &paramFilePath : paramFilePaths) {
    if (paramFilePath != "")
      addCamera(paramFilePath);
  }
}

void Viewport::addCamera(const std::string &path) {
  Logger::getInstance().Success("Added camera: " + path);
  mCameraParameters->Load(path);
  glm::ivec2 resolution = mCameraParameters->ImageCalibratedSize;
  float aspectRatio = float(resolution.x) / resolution.y;
  // int height = 640; // YOLO dimension
  int height = 1640;
  int width = static_cast<int>(height * aspectRatio);
  mCamera = std::make_shared<Camera>(width, height, glm::vec3(0));
  mCamera->SetParameters(*mCameraParameters);
  mCameraManager.AddCamera(mCamera);
  mBgTexture = mTextureManager.getTexture(mCameraParameters->RefImageFilePath);
  mBgQuad->SetTexture(mBgTexture->GetTextureID());
  mFrameBuffer = std::make_shared<FBO>(width, height);
  mCameraManager.AddFBO(mFrameBuffer);
}

void Viewport::removeCamera() {
  bool result = mCameraManager.Remove();
  if (result) {
    std::string msg =
        "Removed camera: " + std::to_string(mCameraManager.GetSelectedId());
    Logger::getInstance().Success(msg);
  } else {
    std::string msg = "Failed to remove camera: " +
                      std::to_string(mCameraManager.GetSelectedId());
    Logger::getInstance().Error(msg);
  }
  switchCamFBO();
}

void Viewport::switchCamFBO() {
  mCamera = mCameraManager.GetCamera();
  if (mCamera == nullptr) {
    mFrameBuffer = nullptr;
    return;
  }
  if (mCamera->GetBgImage() != "") {
    mBgTexture = mTextureManager.getTexture(mCamera->GetBgImage());
    mBgQuad->SetTexture(mBgTexture->GetTextureID());
    mFrameBuffer = mCameraManager.GetFBO();
    Logger::getInstance().Debug("Switch to camera " +
                                std::to_string(mCameraManager.GetSelectedId()));
  }
}

void Viewport::handleOpenModel() {
  const std::vector<std::string> &modelFilePaths =
      FileSystem::OpenFiles(mFolderPath, "Model .obj", "*.obj");
  for (const std::string &modelFilePath : modelFilePaths) {
    if (modelFilePath != "") {
      addModel(modelFilePath);
    }
  }
}

void Viewport::addModel(const std::string &modelPath) {
  std::shared_ptr<Model> model = std::make_shared<Model>(modelPath);
  mModelManager.AddModel(model);
  mPhysicsManager.AddModel(model);
}

void Viewport::removeModel() {
  mPhysicsManager.RemoveBody(mModelManager.GetSelectedId());
  mModelManager.Remove();
}
