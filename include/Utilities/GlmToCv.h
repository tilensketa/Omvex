#pragma once
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>

namespace GlmToCv {
cv::Mat Vec3Mat(const glm::vec3 &vec);
cv::Mat Vec4Mat(const glm::vec4 &vec);
cv::Mat Mat3x3Mat(const glm::mat3x3 &mat);
} // namespace GlmToCv
