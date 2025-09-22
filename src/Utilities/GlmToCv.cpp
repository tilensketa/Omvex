#include "Utilities/GlmToCv.h"

namespace GlmToCv {
cv::Mat Vec3Mat(const glm::vec3 &vec) {
  return cv::Mat(3, 1, CV_32F, (void *)&vec[0]);
}
cv::Mat Vec4Mat(const glm::vec4 &vec) {
  return cv::Mat(4, 1, CV_32F, (void *)&vec[0]);
}
cv::Mat Mat3x3Mat(const glm::mat3x3 &mat) {
  cv::Mat cvMat(3, 3, CV_32F);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      cvMat.at<float>(j, i) = mat[i][j]; // WARN glm column major, cv row major
    }
  }
  return cvMat;
}
} // namespace GlmToCv
