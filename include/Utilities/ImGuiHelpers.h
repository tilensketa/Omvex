#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <imgui.h>
#include <string>
#include <vector>

namespace ImGuiHelpers {
void RecalculateImageSizeOffset(const ImVec2 &windowSize, float aspectRatio,
                                glm::vec2 &size, ImVec2 &offset);
bool ColoredInputFloat(const char *label, float *value, const ImVec4 &color,
                       float width = 100.0f);
bool ColoredDragInt(const char *label, int *value, const ImVec4 &color,
                    float width = 100.0f, float speed = 1.0f, int min = 0,
                    int max = 50);
void CenterText(const std::string &text);
void RenderUIPointsTable(const std::string &tableName,
                         const std::vector<glm::vec2> &imagePos,
                         const std::vector<glm::vec3> &worldPos,
                         const glm::vec2 &imageSize);
void ShowSelectableList(const char *label,
                        const std::vector<std::string> &names, int &selectedId,
                        std::function<void(int)> onSelect = nullptr);
} // namespace ImGuiHelpers
