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

void RenderManager::UpdateRenderClick(const std::string &folderPath) {
  mRenderFolder = FileSystem::SelectFolder(folderPath);
  if (mRenderFolder != "") {
    mRendering = true;
    FileSystem::CreateDir(mRenderFolder + "/segmented");
    FileSystem::CreateDir(mRenderFolder + "/shaded");
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
  std::string subFolder = "segmented/";
  if (mRenderSubId == 0) {
    subFolder = "shaded/";
  }
  std::string path = mRenderFolder + "/" + subFolder + title + ".png";
  saveImage(frameBuffer, path);
  mRenderSubId++;
  if (mRenderSubId == 2) {
    mRenderId++;
    mCameraManager->SwitchNext();
    mRenderSubId = 0;
  }
  if (*mViewMode == ViewMode::Segmented)
    *mViewMode = ViewMode::Shaded;
  else
    *mViewMode = ViewMode::Segmented;

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::string msg = "Render time: " + std::to_string(elapsed.count());
  Logger::getInstance().Info(msg);
}
