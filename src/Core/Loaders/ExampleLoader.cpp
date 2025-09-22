#include "Core/Loader/ExampleLoader.h"

#include <iomanip>
#include <sstream>

ExampleLoader::ExampleLoader(const std::string &exampleFolder)
    : mExampleFolder(exampleFolder) {}

void ExampleLoader::LoadCameras(std::queue<std::string> &cameraQueue) {
  for (size_t i = 1; i < 9; i++) {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << i;
    std::string filename = oss.str();
    std::string cameraPath =
        mExampleFolder + "backgrounds/" + filename + ".json";
    cameraQueue.push(cameraPath);
  }
}
void ExampleLoader::LoadModels(std::queue<std::string> &modelQueue) {
  std::vector<std::string> modelFiles = {
      "Deodorant/deodorant.obj",
      "Hammer/hammer.obj",
      "Light/light.obj",
      "Meter5m/meter5m.obj",
      "Multimeter/multimeter.obj",
      "Power_distributor/power_distributor.obj",
      "Rubiks_cube/rubiks_cube.obj",
      "Sliding_scale/sliding_scale.obj",
      "Uhu/uhu.obj",
      "Wrench/wrench.obj"};
  for (const std::string &modelFile : modelFiles) {
    std::string modelPath = mExampleFolder + "models/" + modelFile;
    modelQueue.push(modelPath);
  }
}
