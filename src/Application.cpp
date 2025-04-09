#include <cmath>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "Application.h"
#include "Texture.h"
#include "Utils.h"

#include <cassert>
#include <memory>

#include <stb_image.h>

Application::Application() : mWindow(initWindow(1000, 1000)) {
  mTimeStep = 0.0f;
  findBaseFolder();
  setWindowIcon();

  initCallbacks();
  setupImGui();
  mIsLoading = std::make_shared<bool>(false);
  mLoadingFracture = std::make_shared<float>(0.0f);

  mTextureManager = std::make_shared<TextureManager>();

  mViewport = std::make_unique<Viewport>(mBaseFolder);
  mCameraCalibrator = std::make_unique<CameraCalibrator>(mBaseFolder);
  mViewport->SetWindow(mWindow.get());
  mActiveFolder = std::make_shared<std::string>("");
  mCameraCalibrator->SetActiveFolder(mActiveFolder);
  mViewport->SetActiveFolder(mActiveFolder);
  mViewport->SetTextureManager(mTextureManager);
  mViewport->SetIsLoading(mIsLoading);
  mViewport->SetLoadingFracture(mLoadingFracture);
  mCameraCalibrator->SetTextureManager(mTextureManager);
  mCameraCalibrator->SetIsLoading(mIsLoading);
  mCameraCalibrator->SetLoadingFracture(mLoadingFracture);
}

Application::~Application() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}

void Application::setWindowIcon() {
  GLFWimage icon;
  std::string icon_path = mBaseFolder + "/resources/OMVEX_icon.png";
  icon.pixels = stbi_load(icon_path.c_str(), &icon.width, &icon.height, 0, 4);

  if (icon.pixels) {
    glfwSetWindowIcon(mWindow.get(), 1, &icon);
    stbi_image_free(icon.pixels);
    Logger::getInstance().Info("Loaded icon: " + icon_path);
  } else {
    Logger::getInstance().Error("Failed to load icon!");
  }
}

void Application::setupImGui() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  // (void)io;
  mImGuiIniFilePath = mConfigsFolder + "imgui.ini";
  io.IniFilename = mImGuiIniFilePath.c_str();
  Logger::getInstance().Debug("imgui.ini file path: " + mImGuiIniFilePath);
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform
  // windows can look identical to regular ones.
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }
  imGuiSetTheme();

  // Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(mWindow.get(), true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::imGuiSetTheme() {
  float col_main = 0.0f;
  float col_area = 0.0f;
  float col_back = 0.0f;
  float col_text = 0.0f;
  if (mTheme == Theme::Light) {
    col_main = 230.f / 255.f;
    col_area = 240.f / 255.f;
    col_back = 250.f / 255.f;
    col_text = 0.0f;
  } else if (mTheme == Theme::Dark) {
    col_main = 40.f / 255.f;
    col_area = 30.f / 255.f;
    col_back = 20.f / 255.f;
    col_text = 1.0f;
  } else if (mTheme == Theme::Black) {
    col_main = 20.f / 255.f;
    col_area = 10.f / 255.f;
    col_back = 0.f / 255.f;
    col_text = 1.0f;
  }

  ImGuiStyle &style = ImGui::GetStyle();

  style.Colors[ImGuiCol_Text] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_text, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_text, 0.58f);
  style.Colors[ImGuiCol_WindowBg] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_back, 1.00f);
  style.Colors[ImGuiCol_ChildBg] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_area, 0.90f);
  style.Colors[ImGuiCol_PopupBg] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_area, 0.90f);

  style.Colors[ImGuiCol_Border] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_text, 0.30f);
  style.Colors[ImGuiCol_BorderShadow] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_text, 0.30f);

  style.Colors[ImGuiCol_FrameBg] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_area, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.68f);
  style.Colors[ImGuiCol_FrameBgActive] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_TitleBg] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.45f);
  style.Colors[ImGuiCol_TitleBgCollapsed] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.35f);
  style.Colors[ImGuiCol_TitleBgActive] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.78f);

  style.Colors[ImGuiCol_MenuBarBg] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_area, 0.57f);
  style.Colors[ImGuiCol_ScrollbarBg] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_area, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrab] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.31f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.78f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_CheckMark] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_text, 1.00f);
  style.Colors[ImGuiCol_SliderGrab] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.24f);
  style.Colors[ImGuiCol_SliderGrabActive] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_Button] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.44f);
  style.Colors[ImGuiCol_ButtonHovered] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.86f);
  style.Colors[ImGuiCol_ButtonActive] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_Header] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.76f);
  style.Colors[ImGuiCol_HeaderHovered] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.86f);
  style.Colors[ImGuiCol_HeaderActive] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
  style.Colors[ImGuiCol_SeparatorHovered] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 1.00f);
  style.Colors[ImGuiCol_SeparatorActive] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.20f);
  style.Colors[ImGuiCol_ResizeGripHovered] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.78f);
  style.Colors[ImGuiCol_ResizeGripActive] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_InputTextCursor] = style.Colors[ImGuiCol_Text];
  style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_HeaderHovered];
  style.Colors[ImGuiCol_Tab] = style.Colors[ImGuiCol_Header];
  style.Colors[ImGuiCol_TabSelected] = style.Colors[ImGuiCol_HeaderActive];
  style.Colors[ImGuiCol_TabSelectedOverline] =
      style.Colors[ImGuiCol_HeaderActive];
  style.Colors[ImGuiCol_TabDimmed] = style.Colors[ImGuiCol_Tab];
  style.Colors[ImGuiCol_TabDimmedSelected] = style.Colors[ImGuiCol_TitleBg];
  style.Colors[ImGuiCol_TabDimmedSelectedOverline] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_back, 1.00f);

  style.Colors[ImGuiCol_PlotLines] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_text, 0.63f);
  style.Colors[ImGuiCol_PlotLinesHovered] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_text, 0.63f);
  style.Colors[ImGuiCol_PlotHistogramHovered] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_main, 0.43f);

  style.Colors[ImGuiCol_DockingEmptyBg] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_area, 1.00f);
  style.Colors[ImGuiCol_DockingPreview] =
      Utils::ImGuiHelpers::GrayAlphaToImVec4(col_area, 1.00f);

  style.WindowRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.GrabRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;
  style.TabRounding = 0.0f;
}

void Application::renderScene() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark gray background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::renderDockingSpaceWithMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Mode")) {
      if (ImGui::MenuItem("CameraCalibration")) {
        mMode = Mode::CameraCalibration;
        Logger::getInstance().Debug("Changed mode to CameraCalibration");
      }
      if (ImGui::MenuItem("Viewport")) {
        mMode = Mode::Viewport3d;
        Logger::getInstance().Debug("Changed mode to Viewport");
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Settings")) {
      if (ImGui::BeginMenu("Themes")) {
        for (size_t i = 0; i < mThemeNames.size(); i++) {
          if (ImGui::MenuItem(mThemeNames[i].c_str())) {
            mTheme = static_cast<Theme>(i);
            imGuiSetTheme();
          }
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Tutorial")) {
        std::string calibrationImagePath =
            mBaseFolder +
            "/resources/screenshots/camera_calibration_dark_09042025.png";
        mCalibrationTutorialImage =
            mTextureManager->getTexture(calibrationImagePath);
        std::string viewportImagePath =
            mBaseFolder + "/resources/screenshots/viewport_shaded_09042025.png";
        mViewportTutorialImage = mTextureManager->getTexture(viewportImagePath);
        mShowTutorial = true;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  // Get the main viewport
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
  ImGui::Begin("Docking Space with Menu Bar", nullptr, window_flags);

  // Adjust the docking space to start below the menu bar
  float menu_bar_height = ImGui::GetFrameHeight(); // Height of the menu bar
  ImGui::SetCursorPosY(menu_bar_height); // Move the cursor below the menu bar
  ImGui::DockSpace(ImGui::GetID("MainDockspace"), ImVec2(0.0f, 0.0f));

  ImGui::End();
  ImGui::PopStyleVar(2); // Restore styles
}

void Application::Run() {
  double fps = 0;
  float ts = 0;
  while (!glfwWindowShouldClose(mWindow.get())) {
    glfwPollEvents();
    mFrameStart = std::chrono::high_resolution_clock::now();

    processInput();

    // Start a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (mShowTutorial) {
      showTutorialWindow();
    }

    ts = 1.0f / ImGui::GetIO().Framerate;
    renderDockingSpaceWithMenuBar();
    if (mMode == Mode::CameraCalibration)
      mCameraCalibrator->Update();
    else if (mMode == Mode::Viewport3d)
      mViewport->Update(ts);

    if (*mIsLoading == true) {
      showLoadingScreen();
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(mWindow.get(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(mWindow.get());

    mFrameEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> frameDuration = mFrameEnd - mFrameStart;
    mTimeStep = static_cast<float>(frameDuration.count());
    fps = 1.0 / mTimeStep;
  }
}

std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>
Application::initWindow(unsigned int width, unsigned int height) {
  if (!glfwInit()) {
    Logger::getInstance().Fatal("Failed to initialize GLFW");
  }
  Logger::getInstance().Success("Successfully initialized GLFW");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  GLFWwindow *window = glfwCreateWindow(width, height, "Omvex", NULL, NULL);
  if (window == NULL) {
    Logger::getInstance().Fatal("Failed to create GLFW window");
  }
  Logger::getInstance().Success("Successfully created GLFW window");
  glfwMakeContextCurrent(window);

  // Disable V-Sync
  glfwSwapInterval(0); // 0 means uncapped, 1 would enable V-Sync

  // Maximize the window
  glfwMaximizeWindow(window);
  // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Logger::getInstance().Fatal("Failed to initialize GLAD");
  }
  Logger::getInstance().Success("Successfully initialized GLAD");
  glViewport(0, 0, width, height);
  return std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>(
      window, glfwDestroyWindow);
}

void Application::processInput() {
  if (glfwGetKey(mWindow.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(mWindow.get(), true);
  if (glfwGetKey(mWindow.get(), GLFW_KEY_Q) == GLFW_PRESS)
    glfwSetWindowShouldClose(mWindow.get(), true);
}

void Application::initCallbacks() {
  glfwSetWindowUserPointer(mWindow.get(), this);
  glfwSetWindowSizeCallback(mWindow.get(), framebuffer_size_callback);
}

void Application::framebuffer_size_callback(GLFWwindow *window, int width,
                                            int height) {
  auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
  if (app) {
    glViewport(0, 0, width, height);
    // app->mWindowSize = glm::ivec2(width, height);
    std::string info = "Screen size changed: " + std::to_string(width) + ", " +
                       std::to_string(height);
    Logger::getInstance().Info(info);
  }
}

void Application::showTutorialWindow() {
  ImGui::Begin("Tutorial", &mShowTutorial);

  ImGui::PushTextWrapPos(ImGui::GetWindowContentRegionMax().x);

  ImGui::Text("Welcome to the tutorial!");
  ImGui::Separator();

  ImGui::TextWrapped("Omvex is program that generates synthetic images. You "
                     "need to have backgrounds and 3d models.");
  ImGui::Separator();

  ImGui::TextWrapped(
      "Omvex has two modes. First one is CameraCalibration, second is "
      "Viewport. You can switch between them in the menubar [Mode].");
  ImGui::TextWrapped(
      "Firstly, we want to configure the camera and get parameters. Switch "
      "mode to [CameraCalibration] if it isn't already.");

  ImGui::Separator();
  ImGui::TextWrapped("CAMERA CALIBRATION");
  ImGui::TextWrapped("In the right window, you see loaded backgrounds. You "
                     "can add a background with [Open image] or add "
                     "preconfigured parameters with [Open params].");

  ImGui::TextWrapped(
      "Once you add a background, a few options will appear. On the main view, "
      "you need to set a planar surface (rectangle) that matches the 3D view. "
      "On the right window, you then set the dimensions of the planar surface "
      "along the x and y axes (units [cm]). You can also view and set grid "
      "points along both "
      "axes. Once you are happy with the configuration, you can save the "
      "parameters. Parameters will be saved in the same folder as the "
      "background image with the same name but a .json extension. You can "
      "configure multiple backgrounds and save them all.");

  ImVec2 windowSize = ImGui::GetContentRegionAvail();
  glm::vec2 textureSize = mCalibrationTutorialImage->GetSize();
  float aspectRatio = textureSize.x / textureSize.y;
  float newWidth = windowSize.x;
  float newHeight = windowSize.x / aspectRatio;
  mCalibrationTutorialImage->Bind();
  ImGui::Image((ImTextureID)(intptr_t)mCalibrationTutorialImage->GetTextureID(),
               ImVec2(newWidth, newHeight));
  mCalibrationTutorialImage->Unbind();

  ImGui::TextWrapped("Now that you have the cameras calibrated, you can switch "
                     "to [Viewport] mode.");

  ImGui::Separator();
  ImGui::TextWrapped("VIEWPORT");
  ImGui::TextWrapped(
      "On the right, you can load cameras that you configured (.json) and also "
      "3D models (.obj). Now that you have all cameras and models in the "
      "scene, you need "
      "to set a couple of options. First is the dimming effect on cameras, and "
      "second is the spawning area where models will be placed. You can also "
      "change the resolution in the menubar [Settings] -> [Resolution] and "
      "test simulation of models with button [Simulate]. Then "
      "you set the number of images you want to render, click [Start Render], and "
      "select a folder.");

  textureSize = mViewportTutorialImage->GetSize();
  aspectRatio = textureSize.x / textureSize.y;
  newWidth = windowSize.x;
  newHeight = windowSize.x / aspectRatio;
  mCalibrationTutorialImage->Bind();
  ImGui::Image((ImTextureID)(intptr_t)mViewportTutorialImage->GetTextureID(),
               ImVec2(newWidth, newHeight));
  mCalibrationTutorialImage->Unbind();

  ImGui::Separator();
  ImGui::PopTextWrapPos();
  ImGui::End();
}

void Application::findBaseFolder() {
  std::vector<std::string> possibleConfigsPaths = {"../configs/",
                                                   "../../configs/"};
  mConfigsFolder = FileSystem::FindExistingFolder(possibleConfigsPaths);
  if (mConfigsFolder == "") {
    Logger::getInstance().Fatal("Base Folder Not Found");
  }
  mBaseFolder = FileSystem::GetDirectoryFromPath(
      FileSystem::GetDirectoryFromPath(mConfigsFolder));
  Logger::getInstance().Debug("Base Folder: " + mBaseFolder);
  Logger::getInstance().Debug("Config Folder: " + mConfigsFolder);
}

void Application::showLoadingScreen() {
  // Get the full screen size
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImVec2 screen_pos = viewport->Pos;
  ImVec2 screen_size = viewport->Size;

  // Fullscreen overlay that captures input
  ImGui::SetNextWindowPos(screen_pos);
  ImGui::SetNextWindowSize(screen_size);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
  ImGui::Begin("##FullscreenBlocker", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoSavedSettings);

  // Draw dimmed background
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRectFilled(
      screen_pos,
      ImVec2(screen_pos.x + screen_size.x, screen_pos.y + screen_size.y),
      IM_COL32(0, 0, 0, 160));

  ImGui::End();
  ImGui::PopStyleVar(2);
  ImGui::PopStyleColor();

  // Centered loading popup
  ImVec2 center = viewport->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowBgAlpha(0.9f);
  ImGui::Begin("##LoadingPopup", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoSavedSettings |
                   ImGuiWindowFlags_NoCollapse);

  ImGui::Text("Loading, please wait...");
  ImGui::Spacing();

  // Optional: Simulate progress
  ImGui::ProgressBar(*mLoadingFracture, ImVec2(200, 20));

  ImGui::End();
}
