#pragma once

#include <string>

struct BaseFolders {
  std::string Root;
  std::string Config;
  std::string Active;
  std::string Example;
  std::string Shaders;
  std::string Resources;
};

class IAppMode {
public:
  virtual ~IAppMode() = default;

  virtual void OnActivate() = 0;
  virtual void OnDeactivate() = 0;
  virtual void Update() = 0;
  virtual void Render() = 0;
  virtual void RenderUI() = 0;
};
