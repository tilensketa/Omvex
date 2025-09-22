#pragma once

#include "Utilities/Serialize.h"

#include <glm/glm.hpp>

struct ImageWorldPos {
  std::vector<glm::vec3> World;
  std::vector<glm::vec2> Image;
};

struct CameraParameters {
  bool DimImage = true;
  bool ShowGrid = true;

  ImageWorldPos GridPos;
  ImageWorldPos RectPos;
  ImageWorldPos CoordSysPos;

  glm::vec2 RCWorldSize;
  glm::ivec2 NumGridPoints;

  glm::vec3 Tvec;
  glm::vec3 Rvec;
  glm::vec3 Translation;
  glm::mat3x3 Rotation;
  glm::mat3x3 Intrinsic;
  glm::vec4 Distortion;

  glm::ivec2 ImageCalibratedSize;

  std::string RefImageFileName;
  std::string Path;

  CameraParameters();

  void Save();
  void LoadJson(const std::string &paramPath);
  void LoadImage(const std::string &imagePath);
  void Dump();

  json ToJson() const;
  void FromJson(const json &j);
};
