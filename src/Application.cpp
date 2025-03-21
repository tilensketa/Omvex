#include <cmath>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "Application.h"
#include "Texture.h"

#include <cassert>
#include <memory>

Application::Application() : mWindow(initWindow(1000, 1000)) {
  mTimeStep = 0.0f;
  initCallbacks();
  setupImGui();
  mViewport.SetWindow(mWindow.get());
  mActiveFolder = std::make_shared<std::string>("");
  mCameraCalibrator.SetActiveFolder(mActiveFolder);
  mViewport.SetActiveFolder(mActiveFolder);
}

Application::~Application() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}

void Application::setupImGui() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  // (void)io;
  std::vector<std::string> possibleConfigsPaths = {"../configs/",
                                                   "../../configs/"};
  mConfigsFolder = FileSystem::FindExistingFolder(possibleConfigsPaths);
  if (mConfigsFolder == "") {
    Logger::getInstance().Fatal("Config folder not found!");
  }
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
      }
      if (ImGui::MenuItem("Viewport")) {
        mMode = Mode::Viewport3d;
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

    ts = 1.0f / ImGui::GetIO().Framerate;
    renderDockingSpaceWithMenuBar();
    if (mMode == Mode::CameraCalibration)
      mCameraCalibrator.Update();
    else if (mMode == Mode::Viewport3d)
      mViewport.Update(ts);

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
