#pragma once

#include <queue>
#include <string>

class ExampleLoader {
public:
  ExampleLoader(const std::string &exampleFolder);

  void LoadCameras(std::queue<std::string> &cameraQueue);
  void LoadModels(std::queue<std::string> &modelQueue);

private:
  std::string mExampleFolder;
};
