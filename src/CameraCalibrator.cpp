#include "CameraCalibrator.h"
#include "Colors.h"
#include "FileSystem.h"
#include "Logger.h"
#include "Utils.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/norm.hpp>
#include <opencv2/opencv.hpp>

void CameraCalibrator::calculateRotationTranslation() {
  std::vector<cv::Point3f> objectPoints(4);
  std::vector<cv::Point2f> imagePoints(4);
  for (size_t i = 0; i < 4; i++) {
    const glm::vec3 &obj = mCameraParameters->RCWorldPos[i];
    const glm::vec2 &img = mCameraParameters->RCImagePos[i];
    glm::vec2 scaled_img = img * mImageSize;
    objectPoints[i] = cv::Point3f(obj.x, obj.y, obj.z);
    imagePoints[i] = cv::Point2f(scaled_img.x, scaled_img.y);
  }
  float step = 100;
  float fx = 50;
  float prev_error = 1;
  float cx = mImageSize.x / 2.0f;
  float cy = mImageSize.y / 2.0f;
  cv::Mat distortion = cv::Mat::zeros(4, 1, CV_32F);
  for (size_t i = 0; i < 100; i++) {
    fx += step;
    float fy = fx;
    cv::Mat intrinsic =
        (cv::Mat_<float>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);

    cv::Mat rvec, tvec;
    cv::solvePnP(objectPoints, imagePoints, intrinsic, distortion, rvec, tvec);

    float error = 0;
    for (size_t j = 0; j < 4; j++) {
      glm::vec2 imgPoint = projectPoint3DTo2D(mCameraParameters->RCWorldPos[j]);
      error += mCameraParameters->RCImagePos[j].x * mImageSize.x - imgPoint.x;
      error += mCameraParameters->RCImagePos[j].y * mImageSize.y - imgPoint.y;
    }
    if ((error < 0) != (prev_error < 0)) {
      step /= -2.0f;
    }
    prev_error = error;

    mCameraParameters->Tvec = Utils::CvMatToGlmVec3(tvec);
    mCameraParameters->Rvec = Utils::CvMatToGlmVec3(rvec);
    mCameraParameters->Intrinsic = Utils::CvMatToGlmMat3x3(intrinsic);
    mCameraParameters->Distortion = Utils::CvMatToGlmVec4(distortion);

    if (glm::abs(step) < 1) {
      break;
    }
  }
  cv::Mat tvec = Utils::GlmVec3ToCvMat(mCameraParameters->Tvec);
  cv::Mat rvec = Utils::GlmVec3ToCvMat(mCameraParameters->Rvec);

  cv::Mat R;
  cv::Rodrigues(rvec, R);

  cv::Mat cameraPosition = -R.t() * tvec;
  cv::Mat cameraRotation = R.t();
  mCameraParameters->Translation = Utils::CvMatToGlmVec3(cameraPosition);
  mCameraParameters->Rotation = Utils::CvMatToGlmMat3x3(cameraRotation);
}

glm::vec2 CameraCalibrator::projectPoint3DTo2D(const glm::vec3 &point3D) {
  std::vector<cv::Point3f> worldPoints = {
      cv::Point3f(point3D.x, point3D.y, point3D.z)};
  cv::Mat tvec = Utils::GlmVec3ToCvMat(mCameraParameters->Tvec);
  cv::Mat rvec = Utils::GlmVec3ToCvMat(mCameraParameters->Rvec);
  cv::Mat intrinsic = Utils::GlmMat3x3ToCvMat(mCameraParameters->Intrinsic);
  cv::Mat distortion = Utils::GlmVec4ToCvMat(mCameraParameters->Distortion);

  std::vector<cv::Point2f> imagePoints;
  cv::projectPoints(worldPoints, rvec, tvec, intrinsic, distortion,
                    imagePoints);
  return glm::vec2(imagePoints[0].x, imagePoints[0].y);
}

glm::vec2 project(const glm::vec2 &A, const glm::vec2 &B) {
  return (glm::dot(A, B) / glm::length2(B)) * B;
}

CameraCalibrator::CameraCalibrator() {
  mCameraParameters = std::make_unique<CameraParameters>("");
  recalculateGridPoints();
}

void CameraCalibrator::Update() {
  recalculateGridPoints();
  recalculate();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open Image")) {
        handleOpenImage();
      }
      if (ImGui::MenuItem("Open Params")) {
        handleOpenParams();
      }
      if (ImGui::MenuItem("Save Params")) {
        std::string fileName =
            FileSystem::GetFileNameFromPath(mLoadedImageFilename);
        std::string saveFolder =
            FileSystem::GetDirectoryFromPath(mLoadedImageFilename);
        std::string saveName =
            FileSystem::RemoveFileExtension(fileName) + ".json";
        std::string savePath = saveFolder + "/" + saveName;
        mCameraParameters->Save(savePath);
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  ImGui::Begin("MainView");
  ImVec2 windowSize = ImGui::GetContentRegionAvail();
  if (mTexture) {
    glm::vec2 textureSize = mTexture->GetSize();
    float aspectRatio = textureSize.x / textureSize.y;
    float newWidth = windowSize.x;
    float newHeight = windowSize.x / aspectRatio;
    if (newHeight > windowSize.y) {
      newHeight = windowSize.y;
      newWidth = windowSize.y * aspectRatio;
    }
    mImageSize = glm::vec2(newWidth, newHeight);
    ImVec2 offset = ImVec2((windowSize.x - mImageSize.x) * 0.5f,
                           (windowSize.y - mImageSize.y) * 0.5f);
    ImGui::SetCursorPos(offset);
    mTexture->Bind();
    ImGui::Image((ImTextureID)(intptr_t)mTexture->GetTextureID(),
                 ImVec2(mImageSize.x, mImageSize.y));
    mTexture->Unbind();
    drag();
    draw();
  } else {
    ImGui::Text("You need to load image");
  }
  ImGui::End();

  ImGui::Begin("Settings");
  ImGui::Text("CAMERA CALIBRATION");
  ImGui::Separator();

  bool changeDim = ImGui::Checkbox("Dim image", &mCameraParameters->DimImage);
  ImGui::SameLine();
  bool changeGrid =
      ImGui::Checkbox("Show grid points", &mCameraParameters->ShowGrid);

  // Rectangle Dimension
  ImGui::Text("Dimension:");
  ImGui::PushItemWidth(100.0f);
  ImGui::PushStyleColor(ImGuiCol_Text, Colors::ImU32ToImVec4(Colors::RED));
  bool changeRCSize0 =
      ImGui::InputFloat("Xd", &mCameraParameters->RCWorldSize[0]);
  ImGui::PopStyleColor();
  ImGui::PopItemWidth();
  ImGui::SameLine();
  ImGui::PushItemWidth(100.0f);
  ImGui::PushStyleColor(ImGuiCol_Text, Colors::ImU32ToImVec4(Colors::GREEN));
  bool changeRCSize1 =
      ImGui::InputFloat("Yd", &mCameraParameters->RCWorldSize[1]);
  ImGui::PopStyleColor();
  ImGui::PopItemWidth();

  // Grid spacing
  ImGui::Text("Grid:");
  ImGui::PushItemWidth(100.0f);
  ImGui::PushStyleColor(ImGuiCol_Text, Colors::ImU32ToImVec4(Colors::RED));
  bool changeGrid0 =
      ImGui::DragInt("Xg", &mCameraParameters->NumGridPoints[0], 1, 1, 50);
  ImGui::PopStyleColor();
  ImGui::PopItemWidth();
  ImGui::SameLine();
  ImGui::PushItemWidth(100.0f);
  ImGui::PushStyleColor(ImGuiCol_Text, Colors::ImU32ToImVec4(Colors::GREEN));
  bool changeGrid1 =
      ImGui::DragInt("Yg", &mCameraParameters->NumGridPoints[1], 1, 1, 50);
  ImGui::PopStyleColor();
  ImGui::PopItemWidth();

  ImGui::Text("Translation");
  ImGui::Text("%s", Utils::GlmVec3ToString(mCameraParameters->Tvec).c_str());
  ImGui::Text("Rotation");
  ImGui::Text("%s", Utils::GlmVec3ToString(mCameraParameters->Rvec).c_str());
  ImGui::Text("Intrinsic");
  ImGui::Text("%s",
              Utils::GlmMat3ToString(mCameraParameters->Intrinsic).c_str());
  ImGui::Text("Distorsion");
  ImGui::Text("%s",
              Utils::GlmVec4ToString(mCameraParameters->Distortion).c_str());
  ImGui::End();
  if (changeRCSize0 || changeRCSize1 || changeGrid0 || changeGrid1 ||
      changeDim || changeGrid)
    mChanged = true;

  ImGui::Begin("Debug");
  if (mTexture)
    ImGui::Text("OriginalImageSize: %f, %f", mTexture->GetSize().x,
                mTexture->GetSize().y);
  ImGui::Text("ImageSize: %f, %f", mImageSize.x, mImageSize.y);
  ImGui::Text("RC Points");
  ImGui::Separator();
  for (size_t i = 0; i < 4; i++) {
    ImU32 color = Colors::GetColorVector()[i + 2];
    ImGui::PushStyleColor(ImGuiCol_Text, Colors::ImU32ToImVec4(color));
    ImGui::Text(
        "World %zu: %s", i,
        Utils::GlmVec2ToString(mCameraParameters->RCWorldPos[i]).c_str());
    ImGui::Text(
        "Scaled Image %zu: %s", i,
        Utils::GlmVec2ToString(mCameraParameters->RCImagePos[i] * mImageSize)
            .c_str());
    ImGui::PopStyleColor();
  }
  ImGui::Text("Coordinate Points");
  ImGui::Separator();
  // ImGui::PushStyleColor(ImGuiCol_Text, Colors::GetColorVector()[7]);
  for (size_t i = 0; i < 4; i++) {
    ImGui::Text(
        "World: %s",
        Utils::GlmVec2ToString(mCameraParameters->CSWorldPos[i]).c_str());
    ImGui::Text(
        "Scaled image: %s",
        Utils::GlmVec2ToString(mCameraParameters->CSImagePos[i] * mImageSize)
            .c_str());
  }
  // ImGui::PopStyleColor();
  ImGui::Text("Grid Points");
  ImGui::Separator();
  ImGui::BeginChild("Scrolling");
  for (size_t i = 0; i < mCameraParameters->GridWorldPos.size(); i++) {
    ImGui::Text(
        "World %zu: %s", i,
        Utils::GlmVec2ToString(mCameraParameters->GridWorldPos[i]).c_str());
    ImGui::Text(
        "Scaled Img %zu: %s", i,
        Utils::GlmVec2ToString(mCameraParameters->GridImagePos[i] * mImageSize)
            .c_str());
  }
  ImGui::EndChild();
  ImGui::End();

  Logger::getInstance().ShowLogs();
}

void CameraCalibrator::draw() {
  ImVec2 imagePos = ImGui::GetItemRectMin();
  ImDrawList *drawList = ImGui::GetWindowDrawList();
  if (mCameraParameters->DimImage) {
    drawList->AddRectFilled(
        imagePos, ImVec2(imagePos.x + mImageSize.x, imagePos.y + mImageSize.y),
        IM_COL32(0, 0, 0, static_cast<int>(255 * 0.5f)));
  }
  // Draw RC lines
  for (size_t i = 0; i < 4; ++i) {
    glm::vec2 start = mCameraParameters->RCImagePos[i] * mImageSize;
    glm::vec2 end = mCameraParameters->RCImagePos[(i + 1) % 4] * mImageSize;

    ImVec2 startPos = ImVec2(imagePos.x + start.x, imagePos.y + start.y);
    ImVec2 endPos = ImVec2(imagePos.x + end.x, imagePos.y + end.y);
    ImU32 color;
    if (i % 2 == 0) {
      color = Colors::GREEN;
    } else {
      color = Colors::RED;
    }
    drawList->AddLine(startPos, endPos, color, 2.0f);
  }
  // Draw RC Circles
  for (size_t i = 0; i < 4; i++) {
    const glm::vec2 &pos = mCameraParameters->RCImagePos[i];
    ImVec2 globalPos = ImVec2(imagePos.x + pos.x * mImageSize.x,
                              imagePos.y + pos.y * mImageSize.y);
    ImU32 color = Colors::GetColorVector()[i + 2];
    drawList->AddCircle(globalPos, mRadius, color, 30, 3);
  }
  // Grid
  if (mCameraParameters->ShowGrid) {
    for (const glm::vec2 &pos : mCameraParameters->GridImagePos) {
      ImVec2 globalPos = ImVec2(imagePos.x + pos.x * mImageSize.x,
                                imagePos.y + pos.y * mImageSize.y);
      drawList->AddCircle(globalPos, mRadius, Colors::WHITE);
    }
  }

  // Coordinate System Lines
  for (size_t i = 0; i < 3; ++i) {
    glm::vec2 start = mCameraParameters->CSImagePos[0];
    glm::vec2 end = mCameraParameters->CSImagePos[(i + 1)];

    ImVec2 startPos = ImVec2(imagePos.x + start.x * mImageSize.x,
                             imagePos.y + start.y * mImageSize.y);
    ImVec2 endPos = ImVec2(imagePos.x + end.x * mImageSize.x,
                           imagePos.y + end.y * mImageSize.y);
    ImU32 color = Colors::GetColorVector()[i];
    drawList->AddLine(startPos, endPos, color, 2.0f);
  }
  // Coordinate System Circles
  for (size_t i = 0; i < 4; i++) {
    const glm::vec2 &pos = mCameraParameters->CSImagePos[i];
    ImVec2 globalPos = ImVec2(imagePos.x + pos.x * mImageSize.x,
                              imagePos.y + pos.y * mImageSize.y);
    ImU32 color;
    if (i == 0) {
      color = Colors::GetColorVector()[3];
    } else {
      color = Colors::GetColorVector()[i - 1];
    }
    drawList->AddCircle(globalPos, mRadius, color, 30, 3);
  }
}

void CameraCalibrator::move(int id) {
  glm::vec2 &pos = mCameraParameters->RCImagePos[id];
  pos.x = glm::clamp(pos.x, 0.0f, 1.0f);
  pos.y = glm::clamp(pos.y, 0.0f, 1.0f);
}

void CameraCalibrator::drag() {
  // Get the image's position and size in the window
  ImVec2 imagePos = ImGui::GetItemRectMin();
  ImVec2 imageMax = ImGui::GetItemRectMax();
  ImVec2 mousePos = ImGui::GetMousePos();
  bool isMouseDown = ImGui::IsMouseDown(0);
  ImVec2 imageSize = ImGui::GetItemRectSize();
  mImageSize = glm::vec2(imageSize.x, imageSize.y);

  for (size_t i = 0; i < 4; i++) {
    glm::vec2 &pos = mCameraParameters->RCImagePos[i];
    char &dragging = mRCImageDrag[i];
    ImVec2 globalPos = ImVec2(imagePos.x + pos.x * mImageSize.x,
                              imagePos.y + pos.y * mImageSize.y);

    // Check if the circle is clicked to start dragging
    if (ImGui::IsMouseClicked(0) && !dragging) {
      float distance = std::sqrt(std::pow(mousePos.x - globalPos.x, 2) +
                                 std::pow(mousePos.y - globalPos.y, 2));
      if (distance <= mRadius) {
        dragging = true;
        break;
      }
    }

    // Dragging logic
    if (dragging && isMouseDown) {
      glm::vec2 newPos =
          glm::vec2(mousePos.x - imagePos.x, mousePos.y - imagePos.y);
      pos = newPos / mImageSize;
      mChanged = true;
    } else {
      dragging = false;
    }
    move(i);
  }
}

void CameraCalibrator::recalculateGridPoints() {
  if (!mChanged)
    return;
  glm::vec2 halfSize = mCameraParameters->RCWorldSize / 2.0f;
  mCameraParameters->GridWorldPos.clear();
  mCameraParameters->GridImagePos.clear();
  const glm::vec2 &numPoints = mCameraParameters->NumGridPoints;
  glm::vec2 dxy =
      mCameraParameters->RCWorldSize / (glm::vec2(numPoints) + glm::vec2(1));
  for (size_t i = 1; i < numPoints.x + 1; i++) {
    for (size_t j = 1; j < numPoints.y + 1; j++) {
      glm::vec2 gridPos = halfSize - glm::vec2(i, j) * dxy;
      mCameraParameters->GridWorldPos.push_back(
          glm::vec3(gridPos.x, gridPos.y, 0));
      mCameraParameters->GridImagePos.push_back(glm::vec2(0));
    }
  }

  mCameraParameters->RCWorldPos[0] = glm::vec3(-halfSize.x, +halfSize.y, 0.0f);
  mCameraParameters->RCWorldPos[1] = glm::vec3(-halfSize.x, -halfSize.y, 0.0f);
  mCameraParameters->RCWorldPos[2] = glm::vec3(+halfSize.x, -halfSize.y, 0.0f);
  mCameraParameters->RCWorldPos[3] = glm::vec3(+halfSize.x, +halfSize.y, 0.0f);
}

void CameraCalibrator::recalculate() {
  if (!mChanged || mLoadedImageFilename == "")
    return;
  mChanged = false;

  // Rescale coordinate system for visibility
  float csSize = (glm::min(mCameraParameters->RCWorldSize.x,
                           mCameraParameters->RCWorldSize.y)) /
                 5.0f;
  mCameraParameters->CSWorldPos[0] = glm::vec3(0, 0, 0);
  mCameraParameters->CSWorldPos[1] = glm::vec3(csSize, 0, 0);
  mCameraParameters->CSWorldPos[2] = glm::vec3(0, csSize, 0);
  mCameraParameters->CSWorldPos[3] = glm::vec3(0, 0, csSize);

  if (mCameraParameters->RCWorldSize.x > 0 &&
      mCameraParameters->RCWorldSize.y > 0)
    calculateRotationTranslation();

  if (mCameraParameters->ShowGrid) {
    for (size_t i = 0; i < mCameraParameters->GridWorldPos.size(); i++) {
      mCameraParameters->GridImagePos[i] =
          projectPoint3DTo2D(mCameraParameters->GridWorldPos[i]) / mImageSize;
    }
  }
  for (size_t i = 0; i < 4; i++) {
    mCameraParameters->CSImagePos[i] =
        projectPoint3DTo2D(mCameraParameters->CSWorldPos[i]) / mImageSize;
  }
  mCameraParameters->ImageCalibratedSize = mImageSize;
}

void CameraCalibrator::handleOpenImage() {
  const std::vector<std::string> &imageFilePaths = FileSystem::OpenFiles(
      mFolderPath, "Image .jpg .jpeg .png", "*.jpg *.jpeg *.png");
  for (const std::string &imageFilePath : imageFilePaths) {
    if (imageFilePath == "")
      continue;

    if (imageFilePath != mLoadedImageFilename) {
      mLoadedImageFilename = imageFilePath;
      mTexture = mTextureManager.getTexture(imageFilePath);
      mCameraParameters->RefImageFilePath =
          FileSystem::GetFileNameFromPath(imageFilePath);
      mChanged = true;
    }
    mFolderPath = imageFilePath;
  }
}

void CameraCalibrator::handleOpenParams() {
  const std::vector<std::string> &paramFilePaths =
      FileSystem::OpenFiles(mFolderPath, "Parameters .json", "*.json");
  for (const std::string &paramFilePath : paramFilePaths) {
    if (paramFilePath == "")
      continue;

    mCameraParameters->Load(paramFilePath);
    const std::string &refImageFilePath =
        FileSystem::GetDirectoryFromPath(paramFilePath) + "/" +
        mCameraParameters->RefImageFilePath;
    if (refImageFilePath != mLoadedImageFilename) {
      mLoadedImageFilename = refImageFilePath;
      mTexture = mTextureManager.getTexture(refImageFilePath);
    }
    mChanged = true;
    mFolderPath = paramFilePath;
  }
}
