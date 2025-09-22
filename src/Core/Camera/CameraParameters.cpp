#include "Core/Camera/CameraParameters.h"

#include "Core/Logger.h"
#include "Utilities/FileSystem.h"
#include "Utilities/GlmToString.h"
#include "Utilities/Serialize.h"
#include <fstream>

namespace Keys {
constexpr auto RefImageFileName = "RefImageFileName";
constexpr auto ImageCalibSize = "ImageCalibrationSize";
constexpr auto RCWorldPos = "RCWorldPos";
constexpr auto RCImagePos = "RCImagePos";
constexpr auto RCWorldSize = "RCWorldSize";
constexpr auto NumGridPoints = "NumGridPoints";
constexpr auto Tvec = "Tvec";
constexpr auto Rvec = "Rvec";
constexpr auto Intrinsic = "Intrinsic";
constexpr auto Distortion = "Distortion";
constexpr auto Translation = "Translation";
constexpr auto Rotation = "Rotation";
} // namespace Keys

CameraParameters::CameraParameters() {
  Path = "";
  RefImageFileName = "";
  RectPos.Image = {
      {0.983, 0.391}, {0.438, 0.072}, {0.076, 0.348}, {0.707, 0.967}};
  RectPos.World.resize(4);
  RCWorldSize = glm::vec2(9, 12);
  NumGridPoints = glm::ivec2(8, 11);
  CoordSysPos.World = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
  CoordSysPos.Image.resize(4);
  Tvec = glm::vec3(0.0f);
  Rvec = glm::vec3(0.0f);
  Translation = glm::vec3(0.0f);
  Rotation = glm::mat3x3(1.0f);
  Intrinsic = glm::mat3x3(1.0f);
  Distortion = glm::vec4(0.0f);
}

// Serialize
json CameraParameters::ToJson() const {
  json j;
  j[Keys::RefImageFileName] = RefImageFileName;
  Serialize::ToJson::Vec(j[Keys::ImageCalibSize], ImageCalibratedSize);
  Serialize::ToJson::Vector(j[Keys::RCWorldPos], RectPos.World);
  Serialize::ToJson::Vector(j[Keys::RCImagePos], RectPos.Image);
  Serialize::ToJson::Vec(j[Keys::RCWorldSize], RCWorldSize);
  Serialize::ToJson::Vec(j[Keys::NumGridPoints], NumGridPoints);
  Serialize::ToJson::Vec(j[Keys::Tvec], Tvec);
  Serialize::ToJson::Vec(j[Keys::Rvec], Rvec);
  Serialize::ToJson::Mat(j[Keys::Intrinsic], Intrinsic);
  Serialize::ToJson::Vec(j[Keys::Distortion], Distortion);
  Serialize::ToJson::Vec(j[Keys::Translation], Translation);
  Serialize::ToJson::Mat(j[Keys::Rotation], Rotation);
  return j;
}

// Deserialize
void CameraParameters::FromJson(const json &j) {
  Serialize::FromJson::SafeString(j, Keys::RefImageFileName, RefImageFileName);
  Serialize::FromJson::SafeVec(j, Keys::ImageCalibSize, ImageCalibratedSize);
  Serialize::FromJson::SafeVector(j, Keys::RCWorldPos, RectPos.World);
  Serialize::FromJson::SafeVector(j, Keys::RCImagePos, RectPos.Image);
  Serialize::FromJson::SafeVec(j, Keys::RCWorldSize, RCWorldSize);
  Serialize::FromJson::SafeVec(j, Keys::NumGridPoints, NumGridPoints);
  Serialize::FromJson::SafeVec(j, Keys::Tvec, Tvec);
  Serialize::FromJson::SafeVec(j, Keys::Rvec, Rvec);
  Serialize::FromJson::SafeMat(j, Keys::Intrinsic, Intrinsic);
  Serialize::FromJson::SafeVec(j, Keys::Distortion, Distortion);
  Serialize::FromJson::SafeVec(j, Keys::Translation, Translation);
  Serialize::FromJson::SafeMat(j, Keys::Rotation, Rotation);

  Dump();
}

void CameraParameters::Dump() {
  Logger::Debug("Loaded camera parameters: " + Path);
  Logger::Debug("RefImageFileName: " + RefImageFileName);
  Logger::Debug("ImageCalibrationSize: " +
                GlmToString::Vec2(ImageCalibratedSize));
  Logger::Debug("RCWorldPos, RCImagePos");
  for (size_t i = 0; i < RectPos.Image.size(); i++) {
    Logger::Debug(GlmToString::Vec3(RectPos.World[i]) + ", " +
                  GlmToString::Vec2(RectPos.Image[i]));
  }
  Logger::Debug("RCWorldSize: " + GlmToString::Vec2(RCWorldSize));
  Logger::Debug("NumGridPoints: " + GlmToString::Vec2(NumGridPoints));
  Logger::Debug("Tvec: " + GlmToString::Vec3(Tvec));
  Logger::Debug("Rvec: " + GlmToString::Vec3(Rvec));
  Logger::Debug("Intrinsic: " + GlmToString::Mat3(Intrinsic));
  Logger::Debug("Distortion: " + GlmToString::Vec4(Distortion));
  Logger::Debug("Translation: " + GlmToString::Vec3(Translation));
  Logger::Debug("Rotation: " + GlmToString::Mat3(Rotation));
}

void CameraParameters::Save() {
  std::ofstream outFile(Path);
  if (!outFile) {
    Logger::Error("CameraParameters: Failed to open file for writing: " + Path);
    return;
  }
  outFile << ToJson().dump(4);
  Logger::Info("CameraParameters: Saved file: " + Path);
}
void CameraParameters::LoadJson(const std::string &filePath) {
  std::ifstream inFile(filePath);
  if (!inFile) {
    Logger::Error("CameraParameters: Failed to open file for reading: " +
                  filePath);
    return;
  }
  json j;
  inFile >> j;
  FromJson(j);
  Path = filePath;
  Logger::Info("CameraParameters: Loaded from: " + filePath);
}
void CameraParameters::LoadImage(const std::string &imagePath) {
  std::string fileName = FileSystem::GetFileNameFromPath(imagePath);
  std::string saveFolder = FileSystem::GetDirectoryFromPath(imagePath);
  std::string saveName = FileSystem::RemoveFileExtension(fileName) + ".json";
  std::string savePath = saveFolder + "/" + saveName;
  Path = savePath;
  RefImageFileName = fileName;
}
