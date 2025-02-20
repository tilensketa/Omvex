#pragma once

#include "Mesh.h"

class MeshManager {
public:
  void AddMesh(std::shared_ptr<Mesh> mesh);
  void Remove();

  void ShowMeshes();

  std::shared_ptr<Mesh> GetMesh() { return mMeshes[mSelectedMeshId]; }
  std::vector<std::shared_ptr<Mesh>> &GetMeshes() { return mMeshes; }
  std::vector<glm::vec3> &GetSegmentedColors() { return mMeshSegmentedColors; }
  const int GetCount() const { return mMeshes.size(); }

  const int GetSelectedId() const { return mSelectedMeshId; }

public:
  static int sMeshId;

private:
  std::vector<std::shared_ptr<Mesh>> mMeshes;
  std::vector<std::string> mMeshNames;
  int mSelectedMeshId = 0;

  std::vector<glm::vec3> mMeshSegmentedColors;
};
