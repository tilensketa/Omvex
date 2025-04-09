#include "Utils.h"

#include <sstream>

#include "Colors.h"

namespace Utils {

// Glm To String Convertion Utils
namespace GlmToString {
std::string Vec2(const glm::vec2 &vec) {
  std::ostringstream oss;
  oss << "[" << vec.x << ", " << vec.y << "]";
  return oss.str();
}
std::string Vec3(const glm::vec3 &vec) {
  std::ostringstream oss;
  oss << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
  return oss.str();
}
std::string Vec4(const glm::vec4 &vec) {
  std::ostringstream oss;
  oss << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.z << "]";
  return oss.str();
}
std::string Mat3(const glm::mat3 &mat) {
  std::ostringstream oss;
  for (int i = 0; i < 3; ++i) {
    oss << "[" << mat[0][i] << ", " << mat[1][i] << ", " << mat[2][i] << "]\n";
  }
  return oss.str();
}
std::string Mat3x4(const glm::mat3x4 &mat) {
  std::ostringstream oss;
  for (int i = 0; i < 3; ++i) {
    oss << "[" << mat[0][i] << ", " << mat[1][i] << ", " << mat[2][i] << ", "
        << mat[3][i] << "]\n";
  }
  return oss.str();
}
std::string Mat4(const glm::mat4 &mat) {
  std::ostringstream oss;
  for (int i = 0; i < 4; ++i) {
    oss << "[" << mat[0][i] << ", " << mat[1][i] << ", " << mat[2][i] << ", "
        << mat[3][i] << "]\n";
  }
  return oss.str();
}
} // namespace GlmToString

// Glm To ImGui Text Conversion Utils
namespace GlmToImGuiText {
// Generic function to print vectors (glm::vec3, glm::vec4, etc.)
template <typename T, size_t N>
void Vec(const std::string &name, const T &vec) {
  static_assert(N > 0 && N <= 4,
                "Only glm::vec2, vec3, or vec4 are supported.");

  ImGui::Text("%s", name.c_str());
  std::string tableName = name + "_Vec";

  if (ImGui::BeginTable(tableName.c_str(), N, ImGuiTableFlags_Borders)) {
    ImGui::TableNextRow();
    for (int i = 0; i < N; i++) {
      ImGui::TableSetColumnIndex(i);
      ImGui::Text("%.3f", vec[i]);
    }
    ImGui::EndTable();
  }
}

// Generic function to print matrices (glm::mat3, glm::mat4, etc.)
template <typename T, size_t N>
void Mat(const std::string &name, const T &mat) {
  static_assert(N > 0 && N <= 4,
                "Only glm::mat2, mat3, or mat4 are supported.");

  ImGui::Text("%s", name.c_str());
  std::string tableName = name + "_Mat";

  if (ImGui::BeginTable(tableName.c_str(), N, ImGuiTableFlags_Borders)) {
    for (int row = 0; row < N; row++) {
      ImGui::TableNextRow();
      for (int col = 0; col < N; col++) {
        ImGui::TableSetColumnIndex(col);
        ImGui::Text("%.3f", mat[col][row]); // GLM is column-major
      }
    }
    ImGui::EndTable();
  }
}

// Convenience functions for specific vector/matrix types
void Vec3(const std::string &name, const glm::vec3 &vec) {
  Vec<glm::vec3, 3>(name, vec);
}
void Vec4(const std::string &name, const glm::vec4 &vec) {
  Vec<glm::vec4, 4>(name, vec);
}
void Mat3(const std::string &name, const glm::mat3 &mat) {
  Mat<glm::mat3, 3>(name, mat);
}
void Mat4(const std::string &name, const glm::mat4 &mat) {
  Mat<glm::mat4, 4>(name, mat);
}

} // namespace GlmToImGuiText

// Glm To Cv Convertion Utils
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

// Cv To Glm Convertion Utils
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

glm::mat4 ReactMat3Vec3ToGlmMat4(const reactphysics3d::Matrix3x3 &orientation,
                                 const reactphysics3d::Vector3 &position) {
  glm::mat4 mat = glm::mat4(1.0f);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      mat[i][j] = orientation[j][i];
    }
  }
  mat[3][0] = position.x;
  mat[3][1] = position.y;
  mat[3][2] = position.z;
  mat[3][3] = 1.0;
  return mat;
}

namespace ImGuiHelpers {
// Generic function for labeled InputFloat with color
bool ColoredInputFloat(const char *label, float *value, const ImVec4 &color,
                       float width) {
  ImGui::PushItemWidth(width);
  ImGui::PushStyleColor(ImGuiCol_Text, color);
  bool changed = ImGui::InputFloat(label, value);
  ImGui::PopStyleColor();
  ImGui::PopItemWidth();
  return changed;
}

// Generic function for labeled DragInt with color
bool ColoredDragInt(const char *label, int *value, const ImVec4 &color,
                    float width, float speed, int min, int max) {
  ImGui::PushItemWidth(width);
  ImGui::PushStyleColor(ImGuiCol_Text, color);
  bool changed = ImGui::DragInt(label, value, speed, min, max);
  ImGui::PopStyleColor();
  ImGui::PopItemWidth();
  return changed;
}

// Helper function to render a table with columns, values, and colors
void RenderPointsTable(const std::string &tableName,
                       const glm::vec3 worldPos[4], const glm::vec2 imagePos[4],
                       const glm::vec2 &imageSize,
                       const std::vector<ImU32> &colors) {
  ImGui::Columns(3, tableName.c_str(), false);
  ImGui::SetColumnWidth(0, 40.0f);

  ImGui::Text("Id");
  ImGui::NextColumn();
  ImGui::Text("World Coordinates");
  ImGui::NextColumn();
  ImGui::Text("Scaled Image Coordinates");
  ImGui::NextColumn();
  ImGui::Separator();

  for (int i = 0; i < 4; i++) {
    ImU32 color = colors[i];
    ImGui::PushStyleColor(ImGuiCol_Text, Colors::ImU32ToImVec4(color));

    ImGui::Text("%i", i);
    ImGui::NextColumn();
    ImGui::Text("%s", Utils::GlmToString::Vec3(worldPos[i]).c_str());
    ImGui::NextColumn();
    ImGui::Text("%s",
                Utils::GlmToString::Vec2(imagePos[i] * imageSize).c_str());
    ImGui::NextColumn();

    ImGui::PopStyleColor();
  }
  ImGui::Columns(1);
  ImGui::Separator();
}
void CenterText(const std::string &text) {
  ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
  ImVec2 windowSize = ImGui::GetContentRegionAvail();
  ImVec2 windowCenter = ImVec2(windowSize.x / 2 - textSize.x / 2,
                               windowSize.y / 2 - textSize.y / 2);
  ImGui::SetCursorPos(windowCenter);
  ImGui::Text("%s", text.c_str());
}
ImVec4 GrayAlphaToImVec4(const float &rgb, const float &alpha) {
  return ImVec4(rgb, rgb, rgb, alpha);
}
} // namespace ImGuiHelpers
} // namespace Utils
