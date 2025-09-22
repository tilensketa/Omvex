#pragma once

#include "Core/CameraCalibrator.h"
#include "Core/Context.h"
#include "Core/Loader/TutorialLoader.h"
#include "Core/Viewport.h"

#include <glad/glad.h>
#include <imgui.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

enum Mode { CameraCalibration, Viewport3d };

class Application {
public:
  Application();

  void Run();

private:
  void switchMode(Mode mode);
  void findBaseFolders();

  void processInput();

  void handleThemeSelection();
  void handleModeSelection();
  void handleTutorialSelection();
  void renderMenuBar();
  void renderDockingSpace();

private:
  bool mRunning = true;
  std::unique_ptr<Context> mContext;

  std::vector<std::string> mModeNames = {"CameraCalibration", "Viewport3D"};
  Mode mCurrentMode;
  IAppMode *mCurrentAppMode;
  std::unique_ptr<CameraCalibrator> mCameraCalibrator;
  std::unique_ptr<Viewport> mViewport;

  std::unique_ptr<BaseFolders> mBaseFolders;

  std::unique_ptr<TextureManager> mTextureManager;
  std::unique_ptr<ExampleLoader> mExampleLoader;
  std::unique_ptr<TutorialLoader> mTutorialLoader;

  Theme mTheme = Theme::Dark;
  std::vector<std::string> mThemeNames = {"Light", "Dark"};
};
