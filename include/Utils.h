#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include <reactphysics3d/reactphysics3d.h>
#include <sstream>
#include <string>

namespace Utils {

inline std::string GlmVec2ToString(const glm::vec2 &vec) {
  std::ostringstream oss;
  oss << "[" << vec.x << ", " << vec.y << "]";
  return oss.str();
}
inline std::string GlmVec3ToString(const glm::vec3 &vec) {
  std::ostringstream oss;
  oss << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
  return oss.str();
}
inline std::string GlmVec4ToString(const glm::vec4 &vec) {
  std::ostringstream oss;
  oss << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.z << "]";
  return oss.str();
}

// Convert glm::mat3 to string
inline std::string GlmMat3ToString(const glm::mat3 &mat) {
  std::ostringstream oss;
  // oss << "[\n";
  for (size_t i = 0; i < 3; ++i) {
    oss << "[" << mat[0][i] << ", " << mat[1][i] << ", " << mat[2][i] << "]\n";
  }
  // oss << "]";
  return oss.str();
}

// Convert glm::mat3 to string
inline std::string GlmMat3x4ToString(const glm::mat3x4 &mat) {
  std::ostringstream oss;
  // oss << "[\n";
  for (size_t i = 0; i < 3; ++i) {
    oss << "[" << mat[0][i] << ", " << mat[1][i] << ", " << mat[2][i] << ", "
        << mat[3][i] << "]\n";
  }
  // oss << "]";
  return oss.str();
}

// Convert glm::mat4 to string
inline std::string GlmMat4ToString(const glm::mat4 &mat) {
  std::ostringstream oss;
  oss << "[\n";
  for (size_t i = 0; i < 4; ++i) {
    oss << "[" << mat[0][i] << ", " << mat[1][i] << ", " << mat[2][i] << ", "
        << mat[3][i] << "]\n";
  }
  oss << "]";
  return oss.str();
}

inline cv::Vec3f GlmToCvVec3(const glm::vec3 &vec) {
  cv::Vec3f cvVec = cv::Vec3f(vec.x, vec.y, vec.z);
  return cvVec;
}

inline cv::Vec4f GlmToCvVec4(const glm::vec4 &vec) {
  cv::Vec4f cvVec = cv::Vec4f(vec.x, vec.y, vec.z, vec.z);
  return cvVec;
}
// Convert glm::vec3 to cv::Mat (3x1 column vector)
inline cv::Mat GlmVec3ToCvMat(const glm::vec3 &vec) {
  return (cv::Mat_<float>(3, 1) << vec.x, vec.y, vec.z);
}

// Convert glm::vec4 to cv::Mat (4x1 column vector)
inline cv::Mat GlmVec4ToCvMat(const glm::vec4 &vec) {
  return (cv::Mat_<float>(4, 1) << vec.x, vec.y, vec.z, vec.w);
}

// Convert glm::mat3x3 to cv::Mat
inline cv::Mat GlmMat3x3ToCvMat(const glm::mat3x3 &mat) {
  // Create a cv::Mat with the same size (3x3) and type (CV_32F)
  cv::Mat cvMat(3, 3, CV_32F);

  // Manually copy the elements of glm::mat3x3 into cv::Mat
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      cvMat.at<float>(j, i) = mat[i][j]; // WARN glm column major, cv row major
    }
  }

  return cvMat;
}

// Convert glm::vec3 to cv::Mat
inline cv::Mat glmToCvMat(const glm::vec3 &vec) {
  // Convert glm::vec3 to a 3x1 cv::Mat
  return cv::Mat(3, 1, CV_32F, (void *)&vec[0]);
}

// Convert cv::Mat to glm::mat3x3
inline glm::mat3x3 cvToGlmMat3x3(const cv::Mat &mat) {
  glm::mat3x3 glmMat;
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
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

inline glm::vec3 CvVec3fToGlmVec3(const cv::Vec3f &vec) {
  return glm::vec3(vec[0], vec[1], vec[2]);
}

inline glm::vec3 CvMatToGlmVec3(const cv::Mat &mat) {
  if (mat.type() == CV_32F) {
    return glm::vec3(mat.at<float>(0, 0), mat.at<float>(1, 0),
                     mat.at<float>(2, 0));
  } else if (mat.type() == CV_64F) {
    return glm::vec3(mat.at<double>(0, 0), mat.at<double>(1, 0),
                     mat.at<double>(2, 0));
  }
  return glm::vec3(0);
}
inline glm::vec4 CvMatToGlmVec4(const cv::Mat &mat) {
  if (mat.type() == CV_32F) {
    return glm::vec4(mat.at<float>(0, 0), mat.at<float>(1, 0),
                     mat.at<float>(2, 0), mat.at<float>(3, 0));
  } else if (mat.type() == CV_64F) {
    return glm::vec4(mat.at<double>(0, 0), mat.at<double>(1, 0),
                     mat.at<double>(2, 0), mat.at<double>(3, 0));
  }
  return glm::vec4(0);
}

inline glm::mat3x3 CvMatToGlmMat3x3(const cv::Mat &mat) {
  glm::mat3x3 glmMat;
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
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

inline glm::vec3 cvToGlmVec3(const cv::Mat &mat) {
  // Ensure the input is a 1x3 or 3x1 matrix
  if (!((mat.rows == 1 && mat.cols == 3) || (mat.rows == 3 && mat.cols == 1))) {
    throw std::invalid_argument("Input cv::Mat must be a 1x3 or 3x1 matrix.");
  }

  glm::vec3 glmVec;

  if (mat.type() == CV_32F) {
    glmVec =
        glm::vec3(mat.at<float>(0, 0),
                  mat.at<float>(mat.rows == 1 ? 0 : 1, mat.rows == 1 ? 1 : 0),
                  mat.at<float>(mat.rows == 1 ? 0 : 2, mat.rows == 1 ? 2 : 0));
  } else if (mat.type() == CV_64F) {
    glmVec = glm::vec3(static_cast<float>(mat.at<double>(0, 0)),
                       static_cast<float>(mat.at<double>(
                           mat.rows == 1 ? 0 : 1, mat.rows == 1 ? 1 : 0)),
                       static_cast<float>(mat.at<double>(
                           mat.rows == 1 ? 0 : 2, mat.rows == 1 ? 2 : 0)));
  } else {
    throw std::invalid_argument(
        "Input cv::Mat must be of type CV_32F or CV_64F.");
  }

  return glmVec;
}

/* // Convert cv::Mat to glm::vec3
inline glm::vec3 cvToGlmVec3(const cv::Mat &mat) {
  return glm::vec3(mat.at<float>(0), mat.at<float>(1), mat.at<float>(2));
} */

inline glm::mat4
ReactMat3Vec3ToGlmMat4(const reactphysics3d::Matrix3x3 &orientation,
                       const reactphysics3d::Vector3 &position) {
  glm::mat4 mat = glm::mat4(1.0f);
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      mat[i][j] = orientation[j][i];
    }
  }
  mat[3][0] = position.x;
  mat[3][1] = position.y;
  mat[3][2] = position.z;
  mat[3][3] = 1.0;
  return mat;
}

} // namespace Utils
