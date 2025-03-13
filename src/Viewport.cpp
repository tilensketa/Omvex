#include "Viewport.h"
#include "Random.h"

#include "Colors.h"
#include "Utils.h"

#define SHADER_PATH "../shaders"

Viewport::Viewport() {
  mCameraParameters = std::make_unique<CameraParameters>("");

  mBgQuad = std::make_unique<Quad>();
  mRayTraceQuad = std::make_unique<Quad>();

  // Load all backgrounds
  for (int i = 1; i < 7; i++) {
    std::string path =
        "/home/tilen/dev/Omvex/test1/" + std::to_string(i) + ".json";
    addCamera(path);
  }

  mFlatShader = std::make_unique<Shader>(SHADER_PATH "/flatVert.glsl",
                                         SHADER_PATH "/flatFrag.glsl");
  mSegmentedShader = std::make_unique<Shader>(
      SHADER_PATH "/segmentedVert.glsl", SHADER_PATH "/segmentedFrag.glsl");
  mShadedShader = std::make_unique<Shader>(SHADER_PATH "/shadedVert.glsl",
                                           SHADER_PATH "/shadedFrag.glsl");
  mBgQuadShader = std::make_unique<Shader>(SHADER_PATH "/quadVert.glsl",
                                           SHADER_PATH "/quadFrag.glsl");
  mRayTraceShader = std::make_unique<Shader>(SHADER_PATH "/rayTraceVert.glsl",
                                             SHADER_PATH "/rayTraceFrag.glsl");

  if (mBgTexture)
    mBgQuad->SetTexture(mBgTexture->GetTextureID());

  {
    mMeshManager.AddMesh(std::make_shared<Mesh>(
        Mesh::CreateCube(16.0f, glm::vec3(0.5f, 0.0f, 0.8f))));
    mMeshManager.AddMesh(std::make_shared<Mesh>(
        Mesh::CreateCube(8.0f, glm::vec3(0.9f, 1.0f, 0.5f))));
    mMeshManager.AddMesh(std::make_shared<Mesh>(
        Mesh::CreateCube(16.0f, glm::vec3(0.5f, 0.9f, 0.9f))));
    mPhysicsManager.AddCube(glm::vec3(8, 8, 8));
    mPhysicsManager.AddCube(glm::vec3(4, 4, 4));
    mPhysicsManager.AddCube(glm::vec3(8, 8, 8));
  }

  mRenderManager.SetViewMode(&mViewMode);
  mRenderManager.SetCameraManager(&mCameraManager);

  glEnable(GL_DEPTH_TEST);

  glm::mat4 mat = glm::mat4(1);
  mat[2][2] = -1;
  mFlatShader->Activate();
  mFlatShader->SetMat4("matrix", mat);
  mShadedShader->Activate();
  mShadedShader->SetMat4("matrix", mat);
  mSegmentedShader->Activate();
  mSegmentedShader->SetMat4("matrix", mat);
  mRayTraceShader->Activate();
  mRayTraceShader->SetMat4("matrix", mat);
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
    ImGui::EndMainMenuBar();
  }

  if (mRenderManager.IsRendering() && !mPhysicsManager.IsSimulating() &&
      (mRenderManager.GetRenderId() % mCameraManager.GetCount() == 0 ||
       mRenderManager.GetRenderId() == 0) &&
      mRenderManager.GetRenderSubId() == 0) {
    mPhysicsManager.SetSimulating(true);
    mPhysicsManager.SetSimulationFrame(0);
  }
  mPhysicsManager.Update(mMeshManager.GetMeshes());

  bool switchCam = mCameraManager.HandleSwitching(mWindow);
  if (switchCam) {
    switchCamFBO();
  }
  renderSceneToFramebuffer();

  bool canRender = !mPhysicsManager.IsSimulating();
  // mRenderManager.Update(mFrameBuffer, canRender); TODO

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
  ImGui::SameLine();
  if (ImGui::RadioButton("Realistic", mViewMode == ViewMode::Realistic)) {
    mViewMode = ViewMode::Realistic;
  }

  mCameraManager.ShowCameras();
  if (ImGui::Button("Add Camera")) {
    handleOpenParams();
  }
  ImGui::SameLine();
  if (ImGui::Button("Remove Camera")) {
    removeCamera();
  }

  mMeshManager.ShowMeshes();
  if (ImGui::Button("Add Mesh Cube")) {
    for (int i = 0; i < 10; i++) {
      glm::vec3 rndColor = Random::Vec3(0, 1);
      float rndSize = Random::Float(4, 16);
      float halfSize = rndSize / 2.0f;
      mMeshManager.AddMesh(
          std::make_shared<Mesh>(Mesh::CreateCube(rndSize, rndColor)));
      mPhysicsManager.AddCube(glm::vec3(halfSize, halfSize, halfSize));
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Add Mesh Sphere")) {
    glm::vec3 rndColor = Random::Vec3(0, 1);
    float radius = Random::Float(2, 8);
    mMeshManager.AddMesh(
        std::make_shared<Mesh>(Mesh::CreateSphere(radius, rndColor)));
    mPhysicsManager.AddSphere(radius);
  }
  ImGui::SameLine();
  if (ImGui::Button("Remove Mesh")) {
    mPhysicsManager.RemoveBody(mMeshManager.GetSelectedId());
    mMeshManager.Remove();
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
  ImGui::Text("Segmented Colors:");
  for (int i = 0; i < mMeshManager.GetCount(); i++) {
    ImGui::ColorEdit3(std::to_string(i).c_str(),
                      &mMeshManager.GetSegmentedColors()[i].x);
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

  if (mViewMode == ViewMode::Flat || mViewMode == ViewMode::Shaded ||
      mViewMode == ViewMode::Realistic) {
    if (mCamera->GetBgImage() != "") {
      glDisable(GL_DEPTH_TEST);
      mBgQuad->Draw(*mBgQuadShader, true);
      glEnable(GL_DEPTH_TEST);
    }
  }
  if (mViewMode == ViewMode::Realistic) {
    mRayTraceQuad->Draw(*mRayTraceShader, false);
  } else {
    for (int i = 0; i < mMeshManager.GetCount(); i++) {
      std::shared_ptr<Mesh> &mesh = mMeshManager.GetMeshes()[i];
      if (mViewMode == ViewMode::Flat) {
        mesh->Draw(*mFlatShader, *mCamera, GL_LINE_STRIP);
      } else if (mViewMode == ViewMode::Shaded) {
        mesh->Draw(*mShadedShader, *mCamera, GL_TRIANGLES);
      } else if (mViewMode == ViewMode::Segmented) {
        mSegmentedShader->Activate();
        glm::vec3 &uniqueColor = mMeshManager.GetSegmentedColors()[i];
        mSegmentedShader->SetVec3("uniqueColor", uniqueColor);
        mesh->Draw(*mSegmentedShader, *mCamera, GL_TRIANGLES);
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
  std::string msg = "Added camera: " + path;
  Logger::getInstance().Log(msg, LogLevel::SUCCESS);
  mCameraParameters->Load(path);
  glm::ivec2 resolution = mCameraParameters->ImageCalibratedSize;
  float aspectRatio = float(resolution.x) / resolution.y;
  int height = 640;
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
    Logger::getInstance().Log(msg, LogLevel::SUCCESS);
  } else {
    std::string msg = "Failed to remove camera: " +
                      std::to_string(mCameraManager.GetSelectedId());
    Logger::getInstance().Log(msg, LogLevel::ERROR);
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
    std::string msg =
        "Switch to camera " + std::to_string(mCameraManager.GetSelectedId());
    Logger::getInstance().Log(msg, LogLevel::DEBUG);
  }
}
