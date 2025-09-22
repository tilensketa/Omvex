#include "Managers/ModelManager.h"

#include "Utilities/FileSystem.h"
#include "Utilities/Colors.h"
#include "Utilities/ImGuiHelpers.h"

#include <imgui.h>

bool ModelManager::isIdValid(int id) {
  return (id >= 0 && id < static_cast<int>(mModels.size()));
}

void ModelManager::AddModel(std::unique_ptr<Model> model) {
  std::string name = FileSystem::GetFileNameFromPath(
      FileSystem::RemoveFileExtension(model->GetPath()));
  mModels.push_back(std::move(model));
  mModelNames.push_back(name);
  mModelSegmentedColors =
      Colors::GenerateSegmentedColors(static_cast<int>(mModels.size()));
  mSelectedId = static_cast<int>(mModels.size()) - 1;
  Logger::Info("ModelManager: Added model " + name);
}

Model *ModelManager::GetModel(int id) {
  if (!isIdValid(id)) return nullptr;
  return mModels[id].get();
}
void ModelManager::Remove(int id) {
  if (!isIdValid(id)) return;

  mModels.erase(mModels.begin() + id);
  mModelNames.erase(mModelNames.begin() + id);

  if (!mModels.empty()) {
    mSelectedId = std::min(mSelectedId, GetCount() - 1);
    mModelSegmentedColors = Colors::GenerateSegmentedColors(GetCount());
  } else {
    mSelectedId = -1;
  }
  Logger::Info("ModelManager: Removed model id: " + std::to_string(id));
}

void ModelManager::ShowModels() {
  ImGuiHelpers::ShowSelectableList("Models", mModelNames, mSelectedId);
}
