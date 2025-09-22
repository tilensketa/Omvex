#include "Utilities/ImGuiHelpers.h"

#include "Utilities/Colors.h"
#include "Utilities/GlmToString.h"

namespace ImGuiHelpers {
void RecalculateImageSizeOffset(const ImVec2 &windowSize, float aspectRatio,
                                glm::vec2 &size, ImVec2 &offset) {
  float newWidth = windowSize.x;
  float newHeight = windowSize.x / aspectRatio;
  if (newHeight > windowSize.y) {
    newHeight = windowSize.y;
    newWidth = windowSize.y * aspectRatio;
  }

  size = glm::vec2(newWidth, newHeight);

  offset =
      ImVec2((windowSize.x - size.x) * 0.5f, (windowSize.y - size.y) * 0.5f);
}
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
void RenderUIPointsTable(const std::string &tableName,
                         const std::vector<glm::vec2> &imagePos,
                         const std::vector<glm::vec3> &worldPos,
                         const glm::vec2 &imageSize) {
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
    ImU32 color = Colors::WHITE;
    ImGui::PushStyleColor(ImGuiCol_Text, Colors::ImU32ToImVec4(color));

    ImGui::Text("%i", i);
    ImGui::NextColumn();
    ImGui::Text("%s", GlmToString::Vec3(worldPos[i]).c_str());
    ImGui::NextColumn();
    ImGui::Text("%s", GlmToString::Vec2(imagePos[i] * imageSize).c_str());
    ImGui::NextColumn();

    ImGui::PopStyleColor();
  }
  ImGui::Columns(1);
  ImGui::Separator();
}
void ShowSelectableList(const char *label,
                        const std::vector<std::string> &names, int &selectedId,
                        std::function<void(int)> onSelect) {
  ImGui::Text("%s", label);
  if (ImGui::BeginListBox(
          (std::string("##") + label).c_str(),
          ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {

    for (size_t n = 0; n < names.size(); n++) {
      const bool is_selected = (selectedId == static_cast<int>(n));
      if (ImGui::Selectable(names[n].c_str(), is_selected) &&
          selectedId != static_cast<int>(n)) {
        selectedId = static_cast<int>(n);
        if (onSelect) onSelect(selectedId); // invoke callback
      }
      if (is_selected) ImGui::SetItemDefaultFocus();
    }
    ImGui::EndListBox();
  }
}
} // namespace ImGuiHelpers
