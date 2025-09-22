#pragma once
#include <glm/glm.hpp>
#include <imgui.h>
#include <string>

namespace GlmToImGuiText {

// Vector table (templated)
template <typename T, size_t N>
void Vec(const std::string &name, const T &vec) {
  static_assert(N > 0 && N <= 4, "Only glm::vec2..vec4 supported");
  ImGui::Text("%s", name.c_str());
  std::string tableName = name + "_Vec";
  if (ImGui::BeginTable(tableName.c_str(), N, ImGuiTableFlags_Borders)) {
    ImGui::TableNextRow();
    for (size_t i = 0; i < N; ++i)
      ImGui::TableSetColumnIndex(i), ImGui::Text("%.3f", vec[i]);
    ImGui::EndTable();
  }
}

// Matrix table (templated)
template <typename T, size_t N>
void Mat(const std::string &name, const T &mat) {
  static_assert(N > 0 && N <= 4, "Only glm::mat2..mat4 supported");
  ImGui::Text("%s", name.c_str());
  std::string tableName = name + "_Mat";
  if (ImGui::BeginTable(tableName.c_str(), N, ImGuiTableFlags_Borders)) {
    for (size_t row = 0; row < N; ++row) {
      ImGui::TableNextRow();
      for (size_t col = 0; col < N; ++col)
        ImGui::TableSetColumnIndex(col), ImGui::Text("%.3f", mat[col][row]);
    }
    ImGui::EndTable();
  }
}

// Convenience
inline void Vec3(const std::string &name, const glm::vec3 &vec) {
  Vec<glm::vec3, 3>(name, vec);
}
inline void Vec4(const std::string &name, const glm::vec4 &vec) {
  Vec<glm::vec4, 4>(name, vec);
}
inline void Mat3(const std::string &name, const glm::mat3 &mat) {
  Mat<glm::mat3, 3>(name, mat);
}
inline void Mat4(const std::string &name, const glm::mat4 &mat) {
  Mat<glm::mat4, 4>(name, mat);
}

} // namespace GlmToImGuiText
