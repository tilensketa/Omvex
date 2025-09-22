#pragma once

#include "Rendering/Models/Model.h"

class ModelManager {
public:
  void AddModel(std::unique_ptr<Model> model);
  void Remove(int id);

  void ShowModels();

  Model *GetSelectedModel() { return mModels[mSelectedId].get(); }
  Model *GetModel(int id);
  std::vector<std::unique_ptr<Model>> &GetModels() { return mModels; }
  const std::vector<glm::vec3> &GetSegmentedColors() const {
    return mModelSegmentedColors;
  }
  const int GetCount() const { return static_cast<int>(mModels.size()); }
  const std::vector<std::string> &GetModelNames() const { return mModelNames; }

  const int GetSelectedId() const { return mSelectedId; }

private:
  bool isIdValid(int id);

private:
  std::vector<std::unique_ptr<Model>> mModels;
  std::vector<std::string> mModelNames;
  int mSelectedId = -1;

  std::vector<glm::vec3> mModelSegmentedColors;
};
