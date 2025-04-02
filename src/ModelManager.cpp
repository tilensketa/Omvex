#include "ModelManager.h"

#include <imgui.h>

#include "Colors.h"

int ModelManager::sModelId = 0;

void ModelManager::AddModel(std::shared_ptr<Model> model) {
  mModels.push_back(model);
  std::string name = model->GetPath() + "_" + std::to_string(sModelId++);
  mModelNames.push_back(name);
  mModelSegmentedColors =
      Colors::GenerateSegmentedColors(static_cast<int>(mModels.size()));
  mSelectedModelId = static_cast<int>(mModels.size()) - 1;
  Logger::getInstance().Info("ModelManager: Added model " + model->GetPath());
}

void ModelManager::Remove() {
  if (mSelectedModelId == -1) {
    return;
  }
  mModels.erase(mModels.begin() + mSelectedModelId);
  mModelNames.erase(mModelNames.begin() + mSelectedModelId);
  if (mModels.size() > 0) {
    mSelectedModelId = 0;
    mModelSegmentedColors =
        Colors::GenerateSegmentedColors(static_cast<int>(mModels.size()));
  } else {
    mSelectedModelId = -1;
  }
  Logger::getInstance().Info("ModelManager: Removed model");
}

void ModelManager::ShowModels() {
  ImGui::Text("Models");
  if (ImGui::BeginListBox(
          "##Models",
          ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
    for (size_t n = 0; n < mModels.size(); n++) {
      const bool is_selected = (mSelectedModelId == n);
      if (ImGui::Selectable(mModelNames[n].c_str(), is_selected) &&
          mSelectedModelId != n) {
        mSelectedModelId = static_cast<int>(n);
      }
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndListBox();
  }
}
