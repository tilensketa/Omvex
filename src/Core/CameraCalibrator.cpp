#include "Core/CameraCalibrator.h"

#include "Core/Camera/CameraMath.h"
#include "Core/Logger.h"

#include "Utilities/Colors.h"
#include "Utilities/FileSystem.h"
#include "Utilities/GlmToImGuiText.h"
#include "Utilities/ImGuiHelpers.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

CameraCalibrator::CameraCalibrator(BaseFolders *folders)
    : mBaseFolders(folders) {
  Logger::Success("CameraCalibrator initialized");
}

void CameraCalibrator::OnActivate() {
  Logger::Info("CameraCalibrator activated");
}

void CameraCalibrator::OnDeactivate() {
  Logger::Info("CameraCalibrator deactivated");
}

void CameraCalibrator::Update() {
  handleMain();
  handleSettings();
  renderDebug();

  handleLoad();
  Logger::ShowLogs();

  if (mChanged && mCameraParameters && mImageSize.x != 0 && mImageSize.y != 0) {
    CameraMath::RecalculateParamPoints(*mCameraParameters);
    CameraMath::Recalculate(*mCameraParameters, mImageSize);
    mChanged = false;
  }
}

void CameraCalibrator::Render() {}

void CameraCalibrator::RenderUI() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::BeginMenu("Open")) {
        if (ImGui::MenuItem("Open Image")) handleOpenImage();
        if (ImGui::MenuItem("Open Params")) handleOpenParams();
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Save")) {
        if (ImGui::MenuItem("Save Params"))
          mCameraParametersManager.SaveSelected();
        if (ImGui::MenuItem("Save All Params"))
          mCameraParametersManager.SaveAll();
        ImGui::EndMenu();
      }
      if (ImGui::MenuItem("Remove")) {
        mCameraParametersManager.Remove(
            mCameraParametersManager.GetSelectedId());
        onParamChange();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Example")) loadExample();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

// SETTINGS
void CameraCalibrator::handleSettings() {
  ImGui::Begin("Settings");
  renderSettings();
  ImGui::End();
}

void CameraCalibrator::renderSettings() {
  ImGui::Text("CAMERA CALIBRATION");
  ImGui::Separator();

  bool paramChange = mCameraParametersManager.ShowParameters();
  if (paramChange) {
    mCameraParameters = mCameraParametersManager.GetCameraParameter();
    onParamChange();
  }

  if (ImGui::Button("Open Image")) handleOpenImage();
  ImGui::SameLine();
  if (ImGui::Button("Open Params")) handleOpenParams();
  ImGui::SameLine();
  if (ImGui::Button("Remove")) {
    mCameraParametersManager.Remove(mCameraParametersManager.GetSelectedId());
    onParamChange();
  }

  if (mCameraParameters) {
    if (ImGui::Checkbox("Dim image", &mCameraParameters->DimImage))
      mCameraParametersManager.SetDimImage(mCameraParameters->DimImage);
    ImGui::SameLine();
    if (ImGui::Checkbox("Show grid", &mCameraParameters->ShowGrid))
      mCameraParametersManager.SetShowGrid(mCameraParameters->ShowGrid);

    ImGui::Text("World Rectangle Dimension:");
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Set world dimensions of planar rectangle");

    ImVec4 red = Colors::ImU32ToImVec4(Colors::RED);
    ImVec4 green = Colors::ImU32ToImVec4(Colors::GREEN);
    if (ImGuiHelpers::ColoredInputFloat(
            "##Xd", &mCameraParameters->RCWorldSize[0], red)) {
      mChanged = true;
    }
    ImGui::SameLine();
    if (ImGuiHelpers::ColoredInputFloat(
            "##Yd", &mCameraParameters->RCWorldSize[1], green)) {
      mChanged = true;
    }

    ImGui::Text("Number Of Grid Points:");
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Set how many grid points to display in each axis");

    if (ImGuiHelpers::ColoredDragInt(
            "##Xg", &mCameraParameters->NumGridPoints[0], red)) {
      mChanged = true;
    }
    ImGui::SameLine();
    if (ImGuiHelpers::ColoredDragInt(
            "##Yg", &mCameraParameters->NumGridPoints[1], green)) {
      mChanged = true;
    }

    if (ImGui::Button("Save Params")) mCameraParametersManager.SaveSelected();
    ImGui::SameLine();
    if (ImGui::Button("Save All Params")) mCameraParametersManager.SaveAll();

    ImGui::Separator();
    ImGui::Separator();

    ImGui::Text("Reference Image Filename: %s",
                mCameraParameters->RefImageFileName.c_str());
    ImGui::Text("CameraParameters Path: %s", mCameraParameters->Path.c_str());
    ImGui::Separator();

    GlmToImGuiText::Vec3("Translation", mCameraParameters->Tvec);
    GlmToImGuiText::Vec3("Rotation", mCameraParameters->Rvec);
    GlmToImGuiText::Mat3("Intrinsics", mCameraParameters->Intrinsic);
    GlmToImGuiText::Vec4("Distorsion", mCameraParameters->Distortion);
  }
}

// DEBUG
void CameraCalibrator::renderDebug() {
  ImGui::Begin("Debug");
  ImGui::Text("Active Folder: %s", mBaseFolders->Active.c_str());
  ImGui::Text("Texture: %s", mTexture ? "valid" : "null");
  ImGui::Text("CameraParams: %s", mCameraParameters ? "valid" : "null");
  ImGui::Text("ImageSize: %f, %f", mImageSize.x, mImageSize.y);
  ImGui::Text("ImageOffset: %f, %f", mImageOffset.x, mImageOffset.y);
  ImGui::Text("LastImagePos: %f, %f", mLastImagePos.x, mLastImagePos.y);
  if (mTexture) {
    ImGui::Text("OriginalImageSize: %f, %f", mTexture->GetSize().x,
                mTexture->GetSize().y);
  }
  if (mCameraParameters) {
    ImGui::Text("Rectangle Coordinate Points");
    ImGuiHelpers::RenderUIPointsTable(
        "RCPointsColumns", mCameraParameters->RectPos.Image,
        mCameraParameters->RectPos.World, mImageSize);

    ImGui::Text("Coordinate System Points");
    ImGuiHelpers::RenderUIPointsTable(
        "CSPointsColumns", mCameraParameters->CoordSysPos.Image,
        mCameraParameters->CoordSysPos.World, mImageSize);
  }
  ImGui::End();
}

// MAIN
void CameraCalibrator::handleMain() {
  ImGui::Begin("Main");
  ImVec2 windowSize = ImGui::GetContentRegionAvail();
  updateMain(windowSize);
  renderMain();
  ImGui::End();
}
void CameraCalibrator::updateMain(const ImVec2 &windowSize) {
  if (!mTexture) return;

  glm::vec2 textureSize = mTexture->GetSize();
  float aspectRatio = textureSize.x / textureSize.y;

  ImGuiHelpers::RecalculateImageSizeOffset(windowSize, aspectRatio, mImageSize,
                                           mImageOffset);
  mLastImagePos = ImVec2(0, 0);
}
void CameraCalibrator::renderMain() {
  if (mTexture && mCameraParameters) {
    ImGui::SetCursorPos(mImageOffset);
    mTexture->Bind();
    ImGui::Image((ImTextureID)(intptr_t)mTexture->GetTextureID(),
                 ImVec2(mImageSize.x, mImageSize.y));
    mTexture->Unbind();

    mLastImagePos = ImGui::GetItemRectMin();
    drag();
    updateOverlayGeometry(mLastImagePos);
    renderOverlay(mLastImagePos);
  } else {
    ImGuiHelpers::CenterText("You need to load image or params");
  }
}

ImVec2 CameraCalibrator::toGlobalPos(const glm::vec2 &normPos,
                                     const ImVec2 &imagePos) const {
  return ImVec2(imagePos.x + normPos.x * mImageSize.x,
                imagePos.y + normPos.y * mImageSize.y);
}
void CameraCalibrator::updateOverlayGeometry(ImVec2 imagePos) {
  mLines.clear();
  mCircles.clear();

  for (size_t i = 0; i < 4; i++) {
    glm::vec2 start = mCameraParameters->RectPos.Image[i];
    glm::vec2 end = mCameraParameters->RectPos.Image[(i + 1) % 4];
    mLines.push_back({toGlobalPos(start, imagePos), toGlobalPos(end, imagePos),
                      (i % 2 == 0) ? Colors::GREEN : Colors::RED});
    mCircles.push_back(
        {toGlobalPos(mCameraParameters->RectPos.Image[i], imagePos), mRadius,
         Colors::WHITE});

    static std::vector<ImU32> CSColors = {Colors::YELLOW, Colors::RED,
                                          Colors::GREEN, Colors::BLUE};
    mCircles.push_back(
        {toGlobalPos(mCameraParameters->CoordSysPos.Image[i], imagePos),
         mRadius / 2.0f, CSColors[i]});
  }
  if (mCameraParameters->ShowGrid) {
    for (const auto &pos : mCameraParameters->GridPos.Image) {
      mCircles.push_back({toGlobalPos(pos, imagePos), mRadius, Colors::WHITE});
    }
  }
  for (size_t i = 0; i < 3; ++i) {
    mLines.push_back(
        {toGlobalPos(mCameraParameters->CoordSysPos.Image[0], imagePos),
         toGlobalPos(mCameraParameters->CoordSysPos.Image[i + 1], imagePos),
         Colors::GetColorsImU32[i]});
  }
}
void CameraCalibrator::renderOverlay(ImVec2 imagePos) {
  ImDrawList *drawList = ImGui::GetWindowDrawList();

  if (mCameraParameters->DimImage) {
    drawList->AddRectFilled(
        imagePos, ImVec2(imagePos.x + mImageSize.x, imagePos.y + mImageSize.y),
        IM_COL32(0, 0, 0, static_cast<int>(255 * 0.5f)));
  }
  for (const auto &line : mLines) {
    drawList->AddLine(line.start, line.end, line.color, 1.0f);
  }
  for (const auto &circle : mCircles) {
    drawList->AddCircle(circle.center, circle.radius, circle.color, 30, 2);
  }
}

void CameraCalibrator::drag() {
  if (!mCameraParameters) return;

  // Get the image's position and size in the window
  ImVec2 imagePos = ImGui::GetItemRectMin();
  ImVec2 imageSize = ImGui::GetItemRectSize();
  ImVec2 mousePos = ImGui::GetMousePos();
  bool isMouseDown = ImGui::IsMouseDown(0);

  for (int i = 0; i < 4; i++) {
    glm::vec2 &pos = mCameraParameters->RectPos.Image[i];
    bool &dragging = mRectImageDrag[i];
    ImVec2 globalPos = ImVec2(imagePos.x + pos.x * imageSize.x,
                              imagePos.y + pos.y * imageSize.y);

    double dx = mousePos.x - globalPos.x;
    double dy = mousePos.y - globalPos.y;
    double distance = std::sqrt(dx * dx + dy * dy);

    if (ImGui::IsMouseClicked(0) && distance <= mRadius) {
      dragging = true;
    }

    // Dragging logic
    if (dragging && isMouseDown) {
      glm::vec2 newPos =
          glm::vec2(mousePos.x - imagePos.x, mousePos.y - imagePos.y);
      pos = newPos / glm::vec2(imageSize.x, imageSize.y);
      mChanged = true;
    } else if (!isMouseDown) {
      dragging = false;
    }
    pos.x = glm::clamp(pos.x, 0.0f, 1.0f);
    pos.y = glm::clamp(pos.y, 0.0f, 1.0f);
  }
}

void CameraCalibrator::handleOpenImage() {
  const std::vector<std::string> &imageFilePaths = FileSystem::OpenFiles(
      mBaseFolders->Active, "Image .jpg .jpeg .png", "*.jpg *.jpeg *.png");
  for (const std::string &imageFilePath : imageFilePaths) {
    if (imageFilePath != "") mCameraImagesLoadingQueue.push(imageFilePath);
  }
}

void CameraCalibrator::handleOpenParams() {
  const std::vector<std::string> &paramFilePaths =
      FileSystem::OpenFiles(mBaseFolders->Active, "Parameters .json", "*.json");
  for (const std::string &paramFilePath : paramFilePaths) {
    if (paramFilePath != "") mCameraParamsLoadingQueue.push(paramFilePath);
  }
}

void CameraCalibrator::addParams(const std::string &paramFile) {
  mCameraParametersManager.AddParameter(paramFile);
  mCameraParameters = mCameraParametersManager.GetCameraParameter();
  onParamChange();
}
void CameraCalibrator::addImage(const std::string &imageFile) {
  mCameraParametersManager.AddImage(imageFile);
  mCameraParameters = mCameraParametersManager.GetCameraParameter();
  onParamChange();
}

void CameraCalibrator::handleLoad() {
  if (!mCameraParamsLoadingQueue.empty()) {
    const std::string paramsPath = mCameraParamsLoadingQueue.front();
    mCameraParamsLoadingQueue.pop();
    addParams(paramsPath);
  }
  if (!mCameraImagesLoadingQueue.empty()) {
    const std::string imagePath = mCameraImagesLoadingQueue.front();
    mCameraImagesLoadingQueue.pop();
    addImage(imagePath);
  }
}

void CameraCalibrator::onParamChange() {
  mCameraParameters = mCameraParametersManager.GetCameraParameter();
  if (!mCameraParameters) {
    mTexture = nullptr;
    mImageSize = glm::vec2(0.0f);
    return;
  }
  const std::string refImageFilePath =
      FileSystem::GetDirectoryFromPath(mCameraParameters->Path) + "/" +
      mCameraParameters->RefImageFileName;

  mTexture = mTextureManager->GetTexture(refImageFilePath);
  mChanged = true;
  Logger::Debug("Camera parameters changed: " + mCameraParameters->Path +
                " | Texture: " + refImageFilePath);
}
void CameraCalibrator::loadExample() {
  mExampleLoader->LoadCameras(mCameraParamsLoadingQueue);
}
