#pragma once

#include "CameraCalibrator.h"
#include "Logger.h"
#include "Viewport.h"

#include <glad/glad.h>
#include <imgui.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <chrono>
#include <memory>

enum Mode { CameraCalibration, Viewport3d };

class Application {
public:
  Application();
  ~Application();

  void Run();

private:
  std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>
  initWindow(unsigned int width, unsigned int height);
  void setupImGui();
  void processInput();

  void renderScene();

  void renderDockingSpaceWithMenuBar();

  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  void initCallbacks();

private:
  std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> mWindow;

  Mode mMode = Mode::CameraCalibration;
  CameraCalibrator mCameraCalibrator;
  Viewport mViewport;

  float mTimeStep;
  std::chrono::time_point<std::chrono::high_resolution_clock> mFrameStart,
      mFrameEnd;

  std::string mConfigsFolder;
  std::string mImGuiIniFilePath;
};
