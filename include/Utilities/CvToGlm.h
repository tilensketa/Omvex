#pragma once
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>

namespace CvToGlm {
glm::vec3 MatVec3(const cv::Mat &mat);
glm::vec4 MatVec4(const cv::Mat &mat);
glm::mat3x3 MatMat3x3(const cv::Mat &mat);
} // namespace CvToGlm
