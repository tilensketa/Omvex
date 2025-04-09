#pragma once

#include <imgui.h>
#include <string>

#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include <reactphysics3d/reactphysics3d.h>

namespace Utils {

// Glm To String Convertion Utils
namespace GlmToString {
std::string Vec2(const glm::vec2 &vec);
std::string Vec3(const glm::vec3 &vec);
std::string Vec4(const glm::vec4 &vec);
std::string Mat3(const glm::mat3 &mat);
std::string Mat3x4(const glm::mat3x4 &mat);
std::string Mat4(const glm::mat4 &mat);
} // namespace GlmToString

// Glm To ImGui Text Conversion Utils
namespace GlmToImGuiText {
template <typename T, size_t N> void Vec(const std::string &name, const T &vec);
template <typename T, size_t N> void Mat(const std::string &name, const T &mat);
void Vec3(const std::string &name, const glm::vec3 &vec);
void Vec4(const std::string &name, const glm::vec4 &vec);
void Mat3(const std::string &name, const glm::mat3 &mat);
void Mat4(const std::string &name, const glm::mat4 &mat);
} // namespace GlmToImGuiText

// Glm To Cv Convertion Utils
namespace GlmToCv {
cv::Mat Vec3Mat(const glm::vec3 &vec);
cv::Mat Vec4Mat(const glm::vec4 &vec);
cv::Mat Mat3x3Mat(const glm::mat3x3 &mat);
} // namespace GlmToCv

// Cv To Glm Convertion Utils
namespace CvToGlm {
glm::vec3 MatVec3(const cv::Mat &mat);
glm::vec4 MatVec4(const cv::Mat &mat);
glm::mat3x3 MatMat3x3(const cv::Mat &mat);
} // namespace CvToGlm

glm::mat4 ReactMat3Vec3ToGlmMat4(const reactphysics3d::Matrix3x3 &orientation,
                                 const reactphysics3d::Vector3 &position);

namespace ImGuiHelpers {
// Generic function for labeled InputFloat with color
bool ColoredInputFloat(const char *label, float *value, const ImVec4 &color,
                       float width = 100.0f);
// Generic function for labeled DragInt with color
bool ColoredDragInt(const char *label, int *value, const ImVec4 &color,
                    float width = 100.0f, float speed = 1, int min = 1,
                    int max = 50);
// Helper function to render a table with columns, values, and colors
void RenderPointsTable(const std::string &tableName,
                       const glm::vec3 worldPos[4], const glm::vec2 imagePos[4],
                       const glm::vec2 &imageSize,
                       const std::vector<ImU32> &colors);
void CenterText(const std::string &text);
ImVec4 GrayAlphaToImVec4(const float &rgb, const float &alpha);
} // namespace ImGuiHelpers
} // namespace Utils
