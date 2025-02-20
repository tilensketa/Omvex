#include "MeshManager.h"

#include "Colors.h"
#include <imgui.h>

int MeshManager::sMeshId = 0;

void MeshManager::AddMesh(std::shared_ptr<Mesh> mesh) {
  mMeshes.push_back(mesh);
  std::string name = mesh->GetName() + "_" + std::to_string(sMeshId++);
  mMeshNames.push_back(name);
  mMeshSegmentedColors = Colors::GenerateSegmentedColors(mMeshes.size());
}

void MeshManager::Remove() {
  if (mMeshes.size() > 0) {
    mMeshes.erase(mMeshes.begin() + mSelectedMeshId);
    mMeshNames.erase(mMeshNames.begin() + mSelectedMeshId);
    mSelectedMeshId = 0;
    mMeshSegmentedColors = Colors::GenerateSegmentedColors(mMeshes.size());
  }
}

void MeshManager::ShowMeshes() {
  ImGui::Text("Meshes");
  if (ImGui::BeginListBox(
          "##Meshes",
          ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
    for (int n = 0; n < mMeshes.size(); n++) {
      const bool is_selected = (mSelectedMeshId == n);
      if (ImGui::Selectable(mMeshNames[n].c_str(), is_selected) &&
          mSelectedMeshId != n) {
        mSelectedMeshId = n;
      }
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndListBox();
  }
}
