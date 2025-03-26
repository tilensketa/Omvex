#include <cmath>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "Application.h"
#include "Texture.h"

#include <cassert>
#include <memory>

#include <stb_image.h>

Application::Application() : mWindow(initWindow(1000, 1000)) {
  mTimeStep = 0.0f;
  findBaseFolder();
  setWindowIcon();

  initCallbacks();
  setupImGui();

  mTextureManager = std::make_shared<TextureManager>();

  mViewport = std::make_unique<Viewport>(mBaseFolder);
  mCameraCalibrator = std::make_unique<CameraCalibrator>(mBaseFolder);
  mViewport->SetWindow(mWindow.get());
  mActiveFolder = std::make_shared<std::string>("");
  mCameraCalibrator->SetActiveFolder(mActiveFolder);
  mViewport->SetActiveFolder(mActiveFolder);
  mViewport->SetTextureManager(mTextureManager);
  mCameraCalibrator->SetTextureManager(mTextureManager);
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
  ImGuiStyle &style = ImGui::GetStyle();
  ImVec4 *colors = style.Colors;

  if (!mIsLightTheme) {
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  } else {
    colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.44f, 0.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 0.44f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.44f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.44f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.44f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.00f, 0.44f, 0.00f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.00f, 0.44f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.00f, 0.44f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
  }

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
      if (ImGui::MenuItem("Light Theme", nullptr, mIsLightTheme)) {
        mIsLightTheme = !mIsLightTheme;
        imGuiSetTheme();
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
            "/resources/screenshots/camera_calibration_dark_26032025.png";
        mCalibrationTutorialImage =
            mTextureManager->getTexture(calibrationImagePath);
        std::string viewportImagePath =
            mBaseFolder + "/resources/screenshots/viewport_shaded_26032025.png";
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
    mTimeStep = frameDuration.count();
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
      "you set the number of images you want to render, click [Render], and "
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
