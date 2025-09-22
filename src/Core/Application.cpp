#include "Core/Application.h"

#include "Core/Logger.h"
#include "Utilities/FileSystem.h"
#include "Utilities/GlmToString.h"

#include <cassert>
#include <chrono>
#include <memory>

Application::Application() {
  findBaseFolders();
  mContext = std::make_unique<Context>(mBaseFolders->Config);

  mTextureManager = std::make_unique<TextureManager>();
  mExampleLoader = std::make_unique<ExampleLoader>(mBaseFolders->Example);
  mTutorialLoader = std::make_unique<TutorialLoader>(mBaseFolders->Resources,
                                                     mTextureManager.get());

  mViewport = std::make_unique<Viewport>(mBaseFolders.get());
  mCameraCalibrator = std::make_unique<CameraCalibrator>(mBaseFolders.get());

  mViewport->SetTextureManager(mTextureManager.get());
  mViewport->SetExampleLoader(mExampleLoader.get());
  mCameraCalibrator->SetTextureManager(mTextureManager.get());
  mCameraCalibrator->SetExampleLoader(mExampleLoader.get());

  mCurrentMode = Mode::CameraCalibration;
  mCurrentAppMode = mCameraCalibrator.get();

  mContext->SetImGuiStyle(mTheme);
}

void Application::Run() {
  float ts = 0;
  while (mRunning) {
    auto frameStart = std::chrono::high_resolution_clock::now();

    mContext->StartFrame();

    processInput();
    renderMenuBar();
    renderDockingSpace();

    mTutorialLoader->ShowTutorialWindow();

    if (mCurrentAppMode) {
      mCurrentAppMode->Update();
      mCurrentAppMode->Render();
      mCurrentAppMode->RenderUI();
    }
    mContext->EndFrame();

    auto frameEnd = std::chrono::high_resolution_clock::now();
    ts = std::chrono::duration<double>(frameEnd - frameStart).count();
  }
  if (mCurrentAppMode) {
    mCurrentAppMode->OnDeactivate();
  }
}

void Application::renderMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    handleModeSelection();
    if (ImGui::BeginMenu("Settings")) {
      handleThemeSelection();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Tutorial")) mTutorialLoader->SetTutorial();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}
void Application::renderDockingSpace() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();

  // Set the window position and size to cover the entire viewport
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);

  // Remove window decorations and set styles
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
      ImGuiWindowFlags_NoDocking;

  // Begin the main docking window
  ImGui::Begin("DockingSpace", nullptr, window_flags);

  // Adjust the docking space to start below the menu bar
  ImGui::SetCursorPosY(ImGui::GetFrameHeight());
  ImGui::DockSpace(ImGui::GetID("MainDockspace"), ImVec2(0.0f, 0.0f));

  ImGui::End();
  ImGui::PopStyleVar(2);
}

void Application::switchMode(Mode mode) {
  if (mCurrentAppMode) mCurrentAppMode->OnDeactivate();

  if (mode == Mode::CameraCalibration)
    mCurrentAppMode = mCameraCalibrator.get();
  else if (mode == Mode::Viewport3d)
    mCurrentAppMode = mViewport.get();

  mCurrentAppMode->OnActivate();
}

void Application::processInput() {
  if (glfwWindowShouldClose(mContext->GetWindow())) mRunning = false;
  if (glfwGetKey(mContext->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    mRunning = false;
  if (glfwGetKey(mContext->GetWindow(), GLFW_KEY_Q) == GLFW_PRESS)
    mRunning = false;
}

void Application::handleThemeSelection() {
  if (ImGui::BeginMenu("Themes")) {
    for (size_t i = 0; i < mThemeNames.size(); i++) {
      bool isActive = (mTheme == static_cast<Theme>(i));
      if (ImGui::MenuItem(mThemeNames[i].c_str(), nullptr, isActive)) {
        mTheme = static_cast<Theme>(i);
        mContext->SetImGuiStyle(mTheme);
      }
    }
    ImGui::EndMenu();
  }
}
void Application::handleModeSelection() {
  if (ImGui::BeginMenu("Mode")) {
    for (size_t i = 0; i < mModeNames.size(); i++) {
      bool isActive = (mCurrentMode == static_cast<Mode>(i));
      if (ImGui::MenuItem(mModeNames[i].c_str(), nullptr, isActive)) {
        mCurrentMode = static_cast<Mode>(i);
        switchMode(mCurrentMode);
      }
    }
    ImGui::EndMenu();
  }
}

void Application::findBaseFolders() {
  std::vector<std::string> possibleConfigsPaths = {"../configs/",
                                                   "../../configs/"};
  std::string configsFolder =
      FileSystem::FindExistingFolder(possibleConfigsPaths);
  if (configsFolder == "") {
    Logger::Fatal("Base Folder Not Found");
  }
  std::string rootFolder = FileSystem::GetDirectoryFromPath(
      FileSystem::GetDirectoryFromPath(configsFolder));
  std::string activeFolder = rootFolder;
  mBaseFolders = std::make_unique<BaseFolders>();
  mBaseFolders->Root = FileSystem::GetDirectoryFromPath(
      FileSystem::GetDirectoryFromPath(configsFolder));
  mBaseFolders->Config = configsFolder;
  mBaseFolders->Active = "";
  mBaseFolders->Example = rootFolder + "/example/";
  mBaseFolders->Shaders = rootFolder + "/shaders/";
  mBaseFolders->Resources = rootFolder + "/resources/";
}
