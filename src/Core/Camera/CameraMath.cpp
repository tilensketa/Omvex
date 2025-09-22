#include "Core/Camera/CameraMath.h"

#include "Utilities/CvToGlm.h"
#include "Utilities/GlmToCv.h"

#include <opencv2/opencv.hpp>
#include <vector>

namespace CameraMath {

glm::vec2 ProjectPoint3DTo2D(const glm::vec3 &point3D,
                             CameraParameters &params) {
  std::vector<cv::Point3f> worldPoints = {
      cv::Point3f(point3D.x, point3D.y, point3D.z)};
  cv::Mat tvec = GlmToCv::Vec3Mat(params.Tvec);
  cv::Mat rvec = GlmToCv::Vec3Mat(params.Rvec);
  cv::Mat intrinsic = GlmToCv::Mat3x3Mat(params.Intrinsic);
  cv::Mat distortion = GlmToCv::Vec4Mat(params.Distortion);

  std::vector<cv::Point2f> imagePoints;
  cv::projectPoints(worldPoints, rvec, tvec, intrinsic, distortion,
                    imagePoints);
  return glm::vec2(imagePoints[0].x, imagePoints[0].y);
}

void CalculateRotationTranslation(CameraParameters &params,
                                  const glm::vec2 &imageSize) {
  std::vector<cv::Point3f> objectPoints(4);
  std::vector<cv::Point2f> imagePoints(4);
  for (size_t i = 0; i < 4; i++) {
    const glm::vec3 &obj = params.RectPos.World[i];
    const glm::vec2 &img = params.RectPos.Image[i];
    glm::vec2 scaled_img = img * imageSize;
    objectPoints[i] = cv::Point3f(obj.x, obj.y, obj.z);
    imagePoints[i] = cv::Point2f(scaled_img.x, scaled_img.y);
  }
  float step = 100;
  float fx = 50;
  float prev_error = 1;
  float cx = imageSize.x / 2.0f;
  float cy = imageSize.y / 2.0f;
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
      glm::vec2 imgPoint = ProjectPoint3DTo2D(params.RectPos.World[j], params);
      error += params.RectPos.Image[j].x * imageSize.x - imgPoint.x;
      error += params.RectPos.Image[j].y * imageSize.y - imgPoint.y;
    }
    if ((error < 0) != (prev_error < 0)) {
      step /= -2.0f;
    }
    prev_error = error;

    params.Tvec = CvToGlm::MatVec3(tvec);
    params.Rvec = CvToGlm::MatVec3(rvec);
    params.Intrinsic = CvToGlm::MatMat3x3(intrinsic);
    params.Distortion = CvToGlm::MatVec4(distortion);

    if (glm::abs(step) < 1) {
      break;
    }
  }
  cv::Mat tvec = GlmToCv::Vec3Mat(params.Tvec);
  cv::Mat rvec = GlmToCv::Vec3Mat(params.Rvec);

  cv::Mat R;
  cv::Rodrigues(rvec, R);

  cv::Mat cameraPosition = -R.t() * tvec;
  cv::Mat cameraRotation = R.t();
  params.Translation = CvToGlm::MatVec3(cameraPosition);
  params.Rotation = CvToGlm::MatMat3x3(cameraRotation);
}
void RecalculateParamPoints(CameraParameters &params) {
  glm::vec2 halfSize = params.RCWorldSize / 2.0f;
  params.GridPos.World.clear();
  params.GridPos.Image.clear();
  const glm::vec2 &numPoints = params.NumGridPoints;
  params.GridPos.World.reserve(numPoints.x * numPoints.y);
  params.GridPos.Image.reserve(numPoints.x * numPoints.y);
  glm::vec2 dxy = params.RCWorldSize / (glm::vec2(numPoints) + glm::vec2(1));
  for (size_t i = 1; i < numPoints.x + 1; i++) {
    for (size_t j = 1; j < numPoints.y + 1; j++) {
      glm::vec2 gridPos = halfSize - glm::vec2(i, j) * dxy;
      params.GridPos.World.push_back(glm::vec3(gridPos.x, gridPos.y, 0));
      params.GridPos.Image.push_back(glm::vec2(0));
    }
  }
  params.RectPos.World[0] = glm::vec3(-halfSize.x, +halfSize.y, 0.0f);
  params.RectPos.World[1] = glm::vec3(-halfSize.x, -halfSize.y, 0.0f);
  params.RectPos.World[2] = glm::vec3(+halfSize.x, -halfSize.y, 0.0f);
  params.RectPos.World[3] = glm::vec3(+halfSize.x, +halfSize.y, 0.0f);
}
void Recalculate(CameraParameters &params, const glm::vec2 &imageSize) {
  // Rescale coordinate system for visibility
  float csSize = (glm::min(params.RCWorldSize.x, params.RCWorldSize.y)) / 5.0f;
  params.CoordSysPos.World[0] = glm::vec3(0, 0, 0);
  params.CoordSysPos.World[1] = glm::vec3(csSize, 0, 0);
  params.CoordSysPos.World[2] = glm::vec3(0, csSize, 0);
  params.CoordSysPos.World[3] = glm::vec3(0, 0, csSize);

  if (params.RCWorldSize.x > 0 && params.RCWorldSize.y > 0) {
    CameraMath::CalculateRotationTranslation(params, imageSize);
  }

  if (params.ShowGrid) {
    for (size_t i = 0; i < params.GridPos.World.size(); i++) {
      glm::vec2 p =
          CameraMath::ProjectPoint3DTo2D(params.GridPos.World[i], params);
      params.GridPos.Image[i] = p / imageSize;
    }
  }
  for (size_t i = 0; i < 4; i++) {
    glm::vec2 p =
        CameraMath::ProjectPoint3DTo2D(params.CoordSysPos.World[i], params);
    params.CoordSysPos.Image[i] = p / imageSize;
  }
  params.ImageCalibratedSize = imageSize;
}
} // namespace CameraMath
