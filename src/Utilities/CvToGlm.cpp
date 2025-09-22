#include "Utilities/CvToGlm.h"

namespace CvToGlm {
glm::vec3 MatVec3(const cv::Mat &mat) {
  if (mat.type() == CV_32F) {
    return glm::vec3(mat.at<float>(0, 0), mat.at<float>(1, 0),
                     mat.at<float>(2, 0));
  } else if (mat.type() == CV_64F) {
    return glm::vec3(mat.at<double>(0, 0), mat.at<double>(1, 0),
                     mat.at<double>(2, 0));
  }
  return glm::vec3(0);
}
glm::vec4 MatVec4(const cv::Mat &mat) {
  if (mat.type() == CV_32F) {
    return glm::vec4(mat.at<float>(0, 0), mat.at<float>(1, 0),
                     mat.at<float>(2, 0), mat.at<float>(3, 0));
  } else if (mat.type() == CV_64F) {
    return glm::vec4(mat.at<double>(0, 0), mat.at<double>(1, 0),
                     mat.at<double>(2, 0), mat.at<double>(3, 0));
  }
  return glm::vec4(0);
}
glm::mat3x3 MatMat3x3(const cv::Mat &mat) {
  glm::mat3x3 glmMat;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (mat.type() == CV_32F) {
        glmMat[j][i] = mat.at<float>(i, j); // GLM is column-major
      } else if (mat.type() == CV_64F) {
        glmMat[j][i] =
            static_cast<float>(mat.at<double>(i, j)); // Convert to float
      }
    }
  }
  return glmMat;
}
} // namespace CvToGlm
