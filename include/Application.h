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
enum Theme { Light, Dark, Black };

class Application {
public:
  Application();
  ~Application();

  void Run();

private:
  std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>
  initWindow(unsigned int width, unsigned int height);
  void setWindowIcon();
  void findBaseFolder();
  void setupImGui();
  void imGuiSetTheme();
  void processInput();

  void renderScene();

  void renderDockingSpaceWithMenuBar();

  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  void initCallbacks();

  void showTutorialWindow();
  void showLoadingScreen();

private:
  std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> mWindow;

  Mode mMode = Mode::CameraCalibration;
  std::unique_ptr<CameraCalibrator> mCameraCalibrator;
  std::unique_ptr<Viewport> mViewport;

  float mTimeStep;
  std::chrono::time_point<std::chrono::high_resolution_clock> mFrameStart,
      mFrameEnd;

  std::string mBaseFolder;
  std::string mConfigsFolder;
  std::string mImGuiIniFilePath;

  std::shared_ptr<TextureManager> mTextureManager;
  std::shared_ptr<Texture> mCalibrationTutorialImage;
  std::shared_ptr<Texture> mViewportTutorialImage;

  Theme mTheme = Theme::Dark;
  std::vector<std::string> mThemeNames = {"Light", "Dark", "Black"};

  std::shared_ptr<std::string> mActiveFolder;

  bool mShowTutorial = false;
  std::shared_ptr<bool> mIsLoading;
  std::shared_ptr<float> mLoadingFracture;
};
