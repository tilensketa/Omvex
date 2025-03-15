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
  const int GetCount() const { return mModels.size(); }

  const int GetSelectedId() const { return mSelectedModelId; }

public:
  static int sModelId;

private:
  std::vector<std::shared_ptr<Model>> mModels;
  std::vector<std::string> mModelNames;
  int mSelectedModelId = -1;

  std::vector<glm::vec3> mModelSegmentedColors;
};
