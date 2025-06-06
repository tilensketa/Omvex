#pragma once

#include "Model.h"

class ModelManager {
public:
  void AddModel(std::shared_ptr<Model> model);
  void Remove();

  void ShowModels();

  std::shared_ptr<Model> GetModel() { return mModels[mSelectedModelId]; }
  std::vector<std::shared_ptr<Model>> &GetModels() { return mModels; }
  std::vector<glm::vec3> &GetSegmentedColors() { return mModelSegmentedColors; }
  const int GetCount() const { return static_cast<int>(mModels.size()); }
  const std::vector<std::string> &GetModelNames() const { return mModelNames; }

  const int GetSelectedId() const { return mSelectedModelId; }

public:
  static int sModelId;

private:
  std::vector<std::shared_ptr<Model>> mModels;
  std::vector<std::string> mModelNames;
  int mSelectedModelId = -1;

  std::vector<glm::vec3> mModelSegmentedColors;
};
