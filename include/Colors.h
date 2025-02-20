#pragma once

#include "Logger.h"
#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtx/color_space.hpp>
#include <vector>

namespace Colors {

// Common color definitions
namespace {
constexpr ImU32 RED = IM_COL32(255, 0, 0, 255);
constexpr ImU32 GREEN = IM_COL32(0, 255, 0, 255);
constexpr ImU32 BLUE = IM_COL32(0, 0, 255, 255);
constexpr ImU32 YELLOW = IM_COL32(255, 255, 0, 255);
constexpr ImU32 CYAN = IM_COL32(0, 255, 255, 255);
constexpr ImU32 MAGENTA = IM_COL32(255, 0, 255, 255);
constexpr ImU32 GRAY = IM_COL32(128, 128, 128, 255);
constexpr ImU32 ORANGE = IM_COL32(255, 165, 0, 255);
constexpr ImU32 PURPLE = IM_COL32(128, 0, 128, 255);
constexpr ImU32 WHITE = IM_COL32(255, 255, 255, 255);
constexpr ImU32 BLACK = IM_COL32(0, 0, 0, 255);

// Log level to color mapping (as ImU32)
constexpr ImU32 INFO_COLOR = IM_COL32(255, 255, 255, 255); // White
constexpr ImU32 SUCCESS_COLOR = IM_COL32(0, 255, 0, 255);  // Green
constexpr ImU32 WARN_COLOR = IM_COL32(255, 255, 0, 255);   // Yellow
constexpr ImU32 ERROR_COLOR = IM_COL32(255, 0, 0, 255);    // Red
constexpr ImU32 FATAL_COLOR = IM_COL32(255, 0, 255, 255);  // Magenta
constexpr ImU32 DEBUG_COLOR = IM_COL32(0, 255, 255, 255);  // Cyan
} // namespace

// Retrieve all colors as a vector
inline const std::vector<ImU32> &GetColorVector() {
  static const std::vector<ImU32> colors = {
      RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, GRAY, ORANGE, PURPLE};
  return colors;
}

// Convert ImVec4 to IM_COL32
inline ImU32 ToImCol32(const ImVec4 &color) {
  return IM_COL32(static_cast<int>(color.x * 255.0f), // Red
                  static_cast<int>(color.y * 255.0f), // Green
                  static_cast<int>(color.z * 255.0f), // Blue
                  static_cast<int>(color.w * 255.0f)  // Alpha
  );
}

// Convert IM_COL32 to ImVec4
inline ImVec4 ToImVec4(ImU32 col32) {
  return ImVec4(((col32 & 0xFF) / 255.0f),         // Red
                (((col32 >> 8) & 0xFF) / 255.0f),  // Green
                (((col32 >> 16) & 0xFF) / 255.0f), // Blue
                (((col32 >> 24) & 0xFF) / 255.0f)  // Alpha
  );
}

inline constexpr ImU32 LogLevelToColor(LogLevel logLevel) {
  switch (logLevel) {
  case LogLevel::INFO:
    return INFO_COLOR;
  case LogLevel::SUCCESS:
    return SUCCESS_COLOR;
  case LogLevel::WARN:
    return WARN_COLOR;
  case LogLevel::ERROR:
    return ERROR_COLOR;
  case LogLevel::FATAL:
    return FATAL_COLOR;
  case LogLevel::DEBUG:
    return DEBUG_COLOR;
  default:
    return INFO_COLOR; // Default to white
  }
}

inline std::vector<glm::vec3> GenerateSegmentedColors(int numMeshes) {
  float s = 1.0f;
  float v = 1.0f;
  std::vector<glm::vec3> colors;
  colors.resize(numMeshes);
  for (int i = 0; i < numMeshes; i++) {
    float h = (float(i) / numMeshes) * 360.0f;
    glm::vec3 hsv = glm::vec3(h, s, v);
    glm::vec3 color = glm::rgbColor(hsv);
    colors[i] = color;
    // colors.push_back(color);
  }
  return colors;
}

} // namespace Colors
