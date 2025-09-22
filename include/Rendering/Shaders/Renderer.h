#pragma once

#include "Core/Camera/Camera.h"
#include "Core/Generator.h"
#include "Rendering/Buffers/FBO.h"
#include "Rendering/Models/Model.h"
#include "Rendering/Models/Quad.h"
#include "Rendering/Shaders/Shader.h"

#include <memory>

class Renderer {
public:
  Renderer(const std::string &shadersPath);

  void Begin(Camera *cam, FBO *fbo, ViewMode *viewMode, Quad *bgQuad);
  void RenderModel(Camera *cam, FBO *fbo, ViewMode *viewMode, Model *model,
                   glm::vec3 &color);
  void End(Quad *dimQuad, float dim, ViewMode *viewMode, FBO *fbo);

private:
  std::unique_ptr<Shader> mRgbShader;
  std::unique_ptr<Shader> mFlatShader;
  std::unique_ptr<Shader> mQuadShader;
  std::unique_ptr<FBO> mPostProcessFBO;
};
