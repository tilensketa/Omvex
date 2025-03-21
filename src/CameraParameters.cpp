#include "CameraParameters.h"
#include "Serialize.h"
#include "Logger.h"

CameraParameters::CameraParameters(const std::string &filePath) {
  Path = filePath;
  RefImageFilePath = filePath;
  Loaded = false;
  std::vector<glm::vec2> positions = {
      {0.983, 0.391}, {0.438, 0.072}, {0.076, 0.348}, {0.707, 0.967}};
  for (size_t i = 0; i < 4; i++) {
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
  Serialize::ToJson(j["ImageCalibrationSize"], ImageCalibratedSize);
  Serialize::ToJson(j["RCWorldPos"], RCWorldPos);
  Serialize::ToJson(j["RCImagePos"], RCImagePos);
  Serialize::ToJson(j["RCWorldSize"], RCWorldSize);
  Serialize::ToJson(j["NumGridPoints"], NumGridPoints);
  Serialize::ToJson(j["Tvec"], Tvec);
  Serialize::ToJson(j["Rvec"], Rvec);
  Serialize::ToJson(j["Intrinsic"], Intrinsic);
  Serialize::ToJson(j["Distortion"], Distortion);
  Serialize::ToJson(j["Translation"], Translation);
  Serialize::ToJson(j["Rotation"], Rotation);
  return j;
}

// Deserialize
void CameraParameters::FromJson(const json &j) {
  RefImageFilePath = j.at("RefImageFilePath").get<std::string>();
  Serialize::FromJson(j.at("ImageCalibrationSize"), ImageCalibratedSize);
  Serialize::FromJson(j.at("RCWorldPos"), RCWorldPos);
  Serialize::FromJson(j.at("RCImagePos"), RCImagePos);
  Serialize::FromJson(j.at("RCWorldSize"), RCWorldSize);
  Serialize::FromJson(j.at("NumGridPoints"), NumGridPoints);
  Serialize::FromJson(j.at("Tvec"), Tvec);
  Serialize::FromJson(j.at("Rvec"), Rvec);
  Serialize::FromJson(j.at("Intrinsic"), Intrinsic);
  Serialize::FromJson(j.at("Distortion"), Distortion);
  Serialize::FromJson(j.at("Translation"), Translation);
  Serialize::FromJson(j.at("Rotation"), Rotation);
}

void CameraParameters::Save() {
  std::ofstream outFile(Path);
  if (outFile.is_open()) {
    outFile << ToJson().dump(4);
    outFile.close();
    Logger::getInstance().Success("CameraParameters: Successfully saved file: " + Path);
  } else {
    Logger::getInstance().Error("CameraParameters: Failed to open file for writing: " + Path);
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
    Logger::getInstance().Success("CameraParameters: Loaded parameters from: " + filePath);
  } else {
    Logger::getInstance().Error("CameraParameters: Failed to open file for reading: " + filePath);
  }
}
