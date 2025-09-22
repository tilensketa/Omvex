#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <GLFW/glfw3.h>
#include <string>

enum class Theme { Light, Dark };

class Context {
public:
  Context(std::string &configsFolder);
  ~Context();
  void StartFrame();
  void EndFrame();

  GLFWwindow *GetWindow() const { return mWindow; }

  void SetImGuiStyle(const Theme &theme);

private:
  void setupImGui();

private:
  GLFWwindow *mWindow = nullptr;
  std::string mConfigsFolder;
};
