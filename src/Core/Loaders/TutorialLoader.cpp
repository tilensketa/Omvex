#include "Core/Loader/TutorialLoader.h"

#include <imgui.h>

TutorialLoader::TutorialLoader(const std::string &resourceFolder,
                               TextureManager *texMng)
    : mResourceFolder(resourceFolder), mTextureManager(texMng) {}

void TutorialLoader::ShowTutorialWindow() {
  if (!mShow) return;

  ImGui::Begin("Tutorial", &mShow);
  ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);

  std::string screenshots = mResourceFolder + "screenshots/";
  std::ifstream file(mResourceFolder + "/tutorial.txt");
  std::string line;
  while (std::getline(file, line)) {
    if (line.rfind("[image:", 0) == 0) {
      std::string filename = line.substr(7, line.size() - 8);
      auto texture = mTextureManager->GetTexture(screenshots + filename);
      if (texture) showImageCentered(texture);
    } else {
      ImGui::TextWrapped("%s", line.c_str());
    }
  }

  ImGui::PopTextWrapPos();
  ImGui::End();
}

void TutorialLoader::showImageCentered(Texture *texture) {
  ImVec2 windowSize = ImGui::GetContentRegionAvail();
  glm::vec2 textureSize = texture->GetSize();
  float aspectRatio = textureSize.x / textureSize.y;
  float newWidth = windowSize.x;
  float newHeight = windowSize.x / aspectRatio;
  texture->Bind();
  ImGui::Image((ImTextureID)(intptr_t)texture->GetTextureID(),
               ImVec2(newWidth, newHeight));
  texture->Unbind();
}
