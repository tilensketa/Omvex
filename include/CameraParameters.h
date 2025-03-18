#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct CameraParameters {
  bool DimImage = true;
  bool ShowGrid = true;

  std::vector<glm::vec3> GridWorldPos;
  std::vector<glm::vec2> GridImagePos;
  glm::vec3 RCWorldPos[4];
  glm::vec2 RCImagePos[4];
  glm::vec3 CSWorldPos[4];
  glm::vec2 CSImagePos[4];

  glm::vec2 RCWorldSize;
  glm::ivec2 NumGridPoints;

  glm::vec3 Tvec;
  glm::vec3 Rvec;
  glm::vec3 Translation;
  glm::mat3x3 Rotation;
  glm::mat3x3 Intrinsic;
  glm::vec4 Distortion;

  glm::ivec2 ImageCalibratedSize;

  std::string RefImageFilePath;
  std::string Path;
  bool Loaded;

  CameraParameters(const std::string &filePath);

  void Save();
  void Load(const std::string &filePath);

  json ToJson();
  void FromJson(const json &j);
};
