#include "Core/Context.h"

#include "Core/Logger.h"
#include "Utilities/Colors.h"
#include "Utilities/GlmToString.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  glViewport(0, 0, width, height);
  std::string info = "Screen size changed: " + std::to_string(width) + ", " +
                     std::to_string(height);
  Logger::Info(info);
}

Context::Context(std::string &configsFolder) : mConfigsFolder(configsFolder) {
  if (!glfwInit()) {
    Logger::Error("Failed to initialize glfw");
  }
  Logger::Success("Initialized glfw");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWmonitor *primary = glfwGetPrimaryMonitor();
  int xpos, ypos, width, height;
  glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
  mWindow = glfwCreateWindow(width, height, "Omvex", nullptr, nullptr);
  Logger::Info("Window pos: " + GlmToString::Vec2(glm::ivec2(xpos, ypos)));
  Logger::Info("Window size: " + GlmToString::Vec2(glm::ivec2(width, height)));

  if (!mWindow) {
    Logger::Error("Failed to initialize window");
  } else {
    Logger::Success("Initialized window");
  }

  glfwMakeContextCurrent(mWindow);
  glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Logger::Error("Failed to initialize glad");
  } else {
    Logger::Success("Initialized glad");
  }

  glfwSwapInterval(0); // 0 means uncapped, 1 would enable V-Sync

  setupImGui();

  Logger::Info("Context initialized");
}

void Context::setupImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  static std::string iniPath = mConfigsFolder + "imgui.ini";
  io.IniFilename = iniPath.c_str();
  Logger::Debug("imgui.ini file path: " + iniPath);

  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 460");

  ImGui::StyleColorsDark();
}

void Context::SetImGuiStyle(const Theme &theme) {
  float col_main = 0.0f;
  float col_area = 0.0f;
  float col_back = 0.0f;
  float col_text = 0.0f;
  if (theme == Theme::Light) {
    col_main = 230.f / 255.f;
    col_area = 240.f / 255.f;
    col_back = 250.f / 255.f;
    col_text = 0.0f;
  } else if (theme == Theme::Dark) {
    col_main = 40.f / 255.f;
    col_area = 30.f / 255.f;
    col_back = 20.f / 255.f;
    col_text = 1.0f;
  }

  ImGuiStyle &style = ImGui::GetStyle();

  style.Colors[ImGuiCol_Text] = Colors::GrayAlphaToImVec4(col_text, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] =
      Colors::GrayAlphaToImVec4(col_text, 0.58f);
  style.Colors[ImGuiCol_WindowBg] = Colors::GrayAlphaToImVec4(col_back, 1.00f);
  style.Colors[ImGuiCol_ChildBg] = Colors::GrayAlphaToImVec4(col_area, 0.90f);
  style.Colors[ImGuiCol_PopupBg] = Colors::GrayAlphaToImVec4(col_area, 0.90f);

  style.Colors[ImGuiCol_Border] = Colors::GrayAlphaToImVec4(col_text, 0.30f);
  style.Colors[ImGuiCol_BorderShadow] =
      Colors::GrayAlphaToImVec4(col_text, 0.30f);

  style.Colors[ImGuiCol_FrameBg] = Colors::GrayAlphaToImVec4(col_area, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered] =
      Colors::GrayAlphaToImVec4(col_main, 0.68f);
  style.Colors[ImGuiCol_FrameBgActive] =
      Colors::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_TitleBg] = Colors::GrayAlphaToImVec4(col_main, 0.45f);
  style.Colors[ImGuiCol_TitleBgCollapsed] =
      Colors::GrayAlphaToImVec4(col_main, 0.35f);
  style.Colors[ImGuiCol_TitleBgActive] =
      Colors::GrayAlphaToImVec4(col_main, 0.78f);

  style.Colors[ImGuiCol_MenuBarBg] = Colors::GrayAlphaToImVec4(col_area, 0.57f);
  style.Colors[ImGuiCol_ScrollbarBg] =
      Colors::GrayAlphaToImVec4(col_area, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrab] =
      Colors::GrayAlphaToImVec4(col_main, 0.31f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] =
      Colors::GrayAlphaToImVec4(col_main, 0.78f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] =
      Colors::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_CheckMark] = Colors::GrayAlphaToImVec4(col_text, 1.00f);
  style.Colors[ImGuiCol_SliderGrab] =
      Colors::GrayAlphaToImVec4(col_main, 0.24f);
  style.Colors[ImGuiCol_SliderGrabActive] =
      Colors::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_Button] = Colors::GrayAlphaToImVec4(col_main, 0.44f);
  style.Colors[ImGuiCol_ButtonHovered] =
      Colors::GrayAlphaToImVec4(col_main, 0.86f);
  style.Colors[ImGuiCol_ButtonActive] =
      Colors::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_Header] = Colors::GrayAlphaToImVec4(col_main, 0.76f);
  style.Colors[ImGuiCol_HeaderHovered] =
      Colors::GrayAlphaToImVec4(col_main, 0.86f);
  style.Colors[ImGuiCol_HeaderActive] =
      Colors::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
  style.Colors[ImGuiCol_SeparatorHovered] =
      Colors::GrayAlphaToImVec4(col_main, 1.00f);
  style.Colors[ImGuiCol_SeparatorActive] =
      Colors::GrayAlphaToImVec4(col_main, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] =
      Colors::GrayAlphaToImVec4(col_main, 0.20f);
  style.Colors[ImGuiCol_ResizeGripHovered] =
      Colors::GrayAlphaToImVec4(col_main, 0.78f);
  style.Colors[ImGuiCol_ResizeGripActive] =
      Colors::GrayAlphaToImVec4(col_main, 1.00f);

  style.Colors[ImGuiCol_InputTextCursor] = style.Colors[ImGuiCol_Text];
  style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_HeaderHovered];
  style.Colors[ImGuiCol_Tab] = style.Colors[ImGuiCol_Header];
  style.Colors[ImGuiCol_TabSelected] = style.Colors[ImGuiCol_HeaderActive];
  style.Colors[ImGuiCol_TabSelectedOverline] =
      style.Colors[ImGuiCol_HeaderActive];
  style.Colors[ImGuiCol_TabDimmed] = style.Colors[ImGuiCol_Tab];
  style.Colors[ImGuiCol_TabDimmedSelected] = style.Colors[ImGuiCol_TitleBg];
  style.Colors[ImGuiCol_TabDimmedSelectedOverline] =
      Colors::GrayAlphaToImVec4(col_back, 1.00f);

  style.Colors[ImGuiCol_PlotLines] = Colors::GrayAlphaToImVec4(col_text, 0.63f);
  style.Colors[ImGuiCol_PlotLinesHovered] =
      Colors::GrayAlphaToImVec4(col_main, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] =
      Colors::GrayAlphaToImVec4(col_text, 0.63f);
  style.Colors[ImGuiCol_PlotHistogramHovered] =
      Colors::GrayAlphaToImVec4(col_main, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] =
      Colors::GrayAlphaToImVec4(col_main, 0.43f);

  style.Colors[ImGuiCol_DockingEmptyBg] =
      Colors::GrayAlphaToImVec4(col_area, 1.00f);
  style.Colors[ImGuiCol_DockingPreview] =
      Colors::GrayAlphaToImVec4(col_area, 1.00f);

  style.WindowRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.GrabRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;
  style.TabRounding = 0.0f;
}

void Context::StartFrame() {
  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // Update viewport size
  int display_w, display_h;
  glfwGetFramebufferSize(mWindow, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
}
void Context::EndFrame() {
  ImGui::Render();
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(mWindow);
}

Context::~Context() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(mWindow);
  glfwTerminate();
}
