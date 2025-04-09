#include "CameraParameters.h"
#include "Logger.h"
#include "Serialize.h"
#include "Utils.h"
#include <fstream>

CameraParameters::CameraParameters(const std::string &filePath) {
  Path = filePath;
  RefImageFilePath = filePath;
  Loaded = false;
  std::vector<glm::vec2> positions = {
      {0.983, 0.391}, {0.438, 0.072}, {0.076, 0.348}, {0.707, 0.967}};
  for (int i = 0; i < 4; i++) {
    RCImagePos[i] = positions[i];
  }
  RCWorldSize = glm::vec2(9, 12);
  NumGridPoints = glm::ivec2(8, 11);
  CSWorldPos[0] = glm::vec3(0, 0, 0);
  CSWorldPos[1] = glm::vec3(1, 0, 0);
  CSWorldPos[2] = glm::vec3(0, 1, 0);
  CSWorldPos[3] = glm::vec3(0, 0, 1);
}

// Serialize
json CameraParameters::ToJson() {
  json j;
  j["RefImageFilePath"] = RefImageFilePath;
  Serialize::ToJson::IVec2(j["ImageCalibrationSize"], ImageCalibratedSize);
  Serialize::ToJson::Vec3Arr4(j["RCWorldPos"], RCWorldPos);
  Serialize::ToJson::Vec2Arr4(j["RCImagePos"], RCImagePos);
  Serialize::ToJson::Vec2(j["RCWorldSize"], RCWorldSize);
  Serialize::ToJson::IVec2(j["NumGridPoints"], NumGridPoints);
  Serialize::ToJson::Vec3(j["Tvec"], Tvec);
  Serialize::ToJson::Vec3(j["Rvec"], Rvec);
  Serialize::ToJson::Mat3x3(j["Intrinsic"], Intrinsic);
  Serialize::ToJson::Vec4(j["Distortion"], Distortion);
  Serialize::ToJson::Vec3(j["Translation"], Translation);
  Serialize::ToJson::Mat3x3(j["Rotation"], Rotation);
  return j;
}

// Deserialize
void CameraParameters::FromJson(const json &j) {
  RefImageFilePath = j.at("RefImageFilePath").get<std::string>();
  Serialize::FromJson::IVec2(j.at("ImageCalibrationSize"), ImageCalibratedSize);
  Serialize::FromJson::Vec3Arr4(j.at("RCWorldPos"), RCWorldPos);
  Serialize::FromJson::Vec2Arr4(j.at("RCImagePos"), RCImagePos);
  Serialize::FromJson::Vec2(j.at("RCWorldSize"), RCWorldSize);
  Serialize::FromJson::IVec2(j.at("NumGridPoints"), NumGridPoints);
  Serialize::FromJson::Vec3(j.at("Tvec"), Tvec);
  Serialize::FromJson::Vec3(j.at("Rvec"), Rvec);
  Serialize::FromJson::Mat3x3(j.at("Intrinsic"), Intrinsic);
  Serialize::FromJson::Vec4(j.at("Distortion"), Distortion);
  Serialize::FromJson::Vec3(j.at("Translation"), Translation);
  Serialize::FromJson::Mat3x3(j.at("Rotation"), Rotation);

  Logger::getInstance().Debug("Loaded camera parameters: " + Path);
  Logger::getInstance().Debug("RefImageFilePath: " + RefImageFilePath);
  Logger::getInstance().Debug("ImageCalibrationSize: " + Utils::GlmToString::Vec2(ImageCalibratedSize));
  Logger::getInstance().Debug("RCWorldPos, RCImagePos");
  for (int i = 0; i < 4; i++){
    Logger::getInstance().Debug(Utils::GlmToString::Vec3(RCWorldPos[i]) + ", " + Utils::GlmToString::Vec2(RCImagePos[i]));
  }
  Logger::getInstance().Debug("RCWorldSize: " + Utils::GlmToString::Vec2(RCWorldSize));
  Logger::getInstance().Debug("NumGridPoints: " + Utils::GlmToString::Vec2(NumGridPoints));
  Logger::getInstance().Debug("Tvec: " + Utils::GlmToString::Vec3(Tvec));
  Logger::getInstance().Debug("Rvec: " + Utils::GlmToString::Vec3(Rvec));
  Logger::getInstance().Debug("Intrinsic: " + Utils::GlmToString::Mat3(Intrinsic));
  Logger::getInstance().Debug("Distorsion: " + Utils::GlmToString::Vec4(Distortion));
  Logger::getInstance().Debug("Translation: " + Utils::GlmToString::Vec3(Translation));
  Logger::getInstance().Debug("Rotation: " + Utils::GlmToString::Mat3(Rotation));
}

void CameraParameters::Save() {
  std::ofstream outFile(Path);
  if (outFile.is_open()) {
    outFile << ToJson().dump(4);
    outFile.close();
    Logger::getInstance().Success(
        "CameraParameters: Successfully saved file: " + Path);
  } else {
    Logger::getInstance().Error(
        "CameraParameters: Failed to open file for writing: " + Path);
  }
}
void CameraParameters::Load(const std::string &filePath) {
  std::ifstream inFile(filePath);
  if (inFile.is_open()) {
    json j;
    inFile >> j;
    inFile.close();
    FromJson(j);
    Path = filePath;
    Loaded = true;
    Logger::getInstance().Success("CameraParameters: Loaded parameters from: " +
                                  filePath);
  } else {
    Logger::getInstance().Error(
        "CameraParameters: Failed to open file for reading: " + filePath);
  }
}
