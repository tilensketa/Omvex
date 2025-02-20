#include "RenderManager.h"
#include "FileSystem.h"

#include <chrono>

void RenderManager::saveImage(std::shared_ptr<FBO> frameBuffer,
                              const std::string &path) {
  if (frameBuffer == nullptr)
    return;
  frameBuffer->colorTexture->Save(path);
  std::string msg = "Saving image: " + path;
  Logger::getInstance().Log(msg, LogLevel::INFO);
}

void RenderManager::Update(std::shared_ptr<FBO> frameBuffer, bool canRender) {
  if (!canRender || frameBuffer == nullptr)
    return;
  if (mRendering) {
    render(frameBuffer);
  }
}

void RenderManager::UpdateRenderClick(const std::string &folderPath) {
  if (mRenderId == 0) {
    mRenderFolder = FileSystem::SelectFolder(folderPath);
    if (mRenderFolder != "") {
      mRendering = true;
    }
  }
}

void RenderManager::render(std::shared_ptr<FBO> frameBuffer) {
  if (mViewMode == nullptr || mCameraManager == nullptr ||
      frameBuffer == nullptr)
    return;
  if (!mRenderSetup) {
    *mViewMode = ViewMode::Shaded;
    mRenderSetup = true;
    mRenderId = 0;
    mRenderSubId = 0;
    return;
  }
  if (mRenderId >= (mRenders)) {
    mRendering = false;
    mRenderSetup = false;
    mRenderId = 0;
    return;
  }
  auto start = std::chrono::high_resolution_clock::now();
  std::string path = mRenderFolder + "/" + std::to_string(mRenderId) + "_" +
                     std::to_string(mRenderSubId) + ".png";
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
  Logger::getInstance().Log(msg, LogLevel::DEBUG);
}
