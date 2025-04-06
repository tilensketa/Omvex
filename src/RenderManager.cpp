#include "RenderManager.h"
#include "FileSystem.h"
#include <iomanip>
#include <sstream>

#include <chrono>

void RenderManager::saveImage(std::shared_ptr<FBO> frameBuffer,
                              const std::string &path) {
  if (frameBuffer == nullptr)
    return;
  frameBuffer->colorTexture->Save(path);
}

void RenderManager::Update(std::shared_ptr<FBO> frameBuffer, bool canRender) {
  if (!canRender || frameBuffer == nullptr)
    return;
  if (mRendering) {
    render(frameBuffer);
  }
}

void RenderManager::UpdateRenderClick(
    const std::string &folderPath, const std::vector<glm::vec3> &colors,
    const std::vector<std::string> &modelNames) {
  mRenderFolder = FileSystem::SelectFolder(folderPath);
  if (mRenderFolder != "") {
    mRendering = true;
    FileSystem::CreateDir(mRenderFolder + "/segmented2d");
    FileSystem::CreateDir(mRenderFolder + "/segmented3d");
    FileSystem::CreateDir(mRenderFolder + "/shaded");
    Texture uniqueColors = Texture(colors);
    uniqueColors.Save(mRenderFolder + "/unique_colors.png");

    std::string modelNamesPath = mRenderFolder + "/model_names.txt";
    std::ofstream file(modelNamesPath);
    if (!file.is_open()) {
      Logger::getInstance().Error("Failed to open model names path file: " +
                                  modelNamesPath);
    } else {
      for (const std::string &modelName : modelNames) {
        file << modelName << std::endl;
      }
      file.close();
    }
    Logger::getInstance().Info("Start Rendering");
  }
}

void RenderManager::StopRendering() {
  Logger::getInstance().Info("Rendering Stopped");
  mRendering = false;
  mRenderSetup = false;
  mRenderId = 0;
  mRenderSubId = 0;
}

void RenderManager::render(std::shared_ptr<FBO> frameBuffer) {
  if (mViewMode == nullptr || mCameraManager == nullptr ||
      frameBuffer == nullptr)
    return;
  // Setup for render start
  if (!mRenderSetup) {
    *mViewMode = ViewMode::Shaded;
    mRenderSetup = true;
    return;
  }
  if (mRenderId >= mRenders) {
    StopRendering();
    return;
  }
  auto start = std::chrono::high_resolution_clock::now();
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(5) << mRenderId;
  std::string title = oss.str();
  std::string subFolder = "";
  if (mRenderSubId == 0) {
    subFolder = "shaded/";
  } else if (mRenderSubId == 1) {
    subFolder = "segmented2d/";
  } else {
    subFolder = "segmented3d/";
  }
  std::string path = mRenderFolder + "/" + subFolder + title + ".png";
  saveImage(frameBuffer, path);
  mRenderSubId++;
  if (mRenderSubId == 3) {
    mRenderId++;
    mCameraManager->SwitchNext();
    mRenderSubId = 0;
  }
  if (*mViewMode == ViewMode::Shaded)
    *mViewMode = ViewMode::Segmented2D;
  else if (*mViewMode == ViewMode::Segmented2D)
    *mViewMode = ViewMode::Segmented3D;
  else
    *mViewMode = ViewMode::Shaded;

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::string msg = "Render time: " + std::to_string(elapsed.count());
  Logger::getInstance().Info(msg);
}
