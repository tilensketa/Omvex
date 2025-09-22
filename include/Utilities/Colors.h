#pragma once

#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtx/color_space.hpp>
#include <vector>

namespace Colors {

namespace ANSI {
constexpr const char *RESET = "\033[0m";
constexpr const char *RED = "\033[31m";
constexpr const char *GREEN = "\033[32m";
constexpr const char *YELLOW = "\033[33m";
constexpr const char *BLUE = "\033[34m";
constexpr const char *MAGENTA = "\033[35m";
constexpr const char *CYAN = "\033[36m";
constexpr const char *WHITE = "\033[37m";
} // namespace ANSI

// Common color definitions
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
constexpr ImU32 INFO_COLOR = GRAY;
constexpr ImU32 SUCCESS_COLOR = GREEN;
constexpr ImU32 WARN_COLOR = YELLOW;
constexpr ImU32 ERROR_COLOR = RED;
constexpr ImU32 FATAL_COLOR = MAGENTA;
constexpr ImU32 DEBUG_COLOR = CYAN;

constexpr ImU32 GetLogColorsImU32[] = {INFO_COLOR,  WARN_COLOR,    ERROR_COLOR,
                                       DEBUG_COLOR, SUCCESS_COLOR, FATAL_COLOR};
constexpr ImU32 GetColorsImU32[]{RED,     GREEN, BLUE,   YELLOW, CYAN,
                                 MAGENTA, GRAY,  ORANGE, PURPLE};

// Convert IM_COL32 to ImVec4
inline ImVec4 ImU32ToImVec4(ImU32 col32) {
  return ImVec4(((col32 & 0xFF) / 255.0f),         // Red
                (((col32 >> 8) & 0xFF) / 255.0f),  // Green
                (((col32 >> 16) & 0xFF) / 255.0f), // Blue
                (((col32 >> 24) & 0xFF) / 255.0f)  // Alpha
  );
}

inline std::vector<glm::vec3> GenerateSegmentedColors(int numMeshes) {
  float s = 1.0f;
  float v = 1.0f;
  std::vector<glm::vec3> colors;
  colors.resize(numMeshes);
  for (size_t i = 0; i < numMeshes; i++) {
    float h = (float(i) / numMeshes) * 360.0f;
    glm::vec3 hsv = glm::vec3(h, s, v);
    glm::vec3 color = glm::rgbColor(hsv);
    color.r = round(color.r * 255.0f) / 255.0f;
    color.g = round(color.g * 255.0f) / 255.0f;
    color.b = round(color.b * 255.0f) / 255.0f;
    colors[i] = color;
  }
  return colors;
}

inline ImVec4 GrayAlphaToImVec4(const float &rgb, const float &alpha) {
  return ImVec4(rgb, rgb, rgb, alpha);
}
} // namespace Colors
