#include "Rendering/Shaders/Renderer.h"

Renderer::Renderer(const std::string &shadersPath) {
  mRgbShader =
      std::make_unique<Shader>(shadersPath, "colorVert.glsl", "colorFrag.glsl");
  mFlatShader =
      std::make_unique<Shader>(shadersPath, "flatVert.glsl", "flatFrag.glsl");
  mQuadShader =
      std::make_unique<Shader>(shadersPath, "quadVert.glsl", "quadFrag.glsl");
  mPostProcessFBO = std::make_unique<FBO>(100, 100);

  glDisable(GL_DITHER);
  glDisable(GL_BLEND);
  glDisable(GL_MULTISAMPLE);
}

void Renderer::Begin(Camera *cam, FBO *fbo, ViewMode *viewMode, Quad *bgQuad) {
  if (!cam || !fbo) return;

  glEnable(GL_DEPTH_TEST);
  glm::mat4 mat = glm::mat4(1.0f);
  mat[2][2] = -1.0f;
  mRgbShader->Activate();
  mRgbShader->SetMat4("uMatrix", mat);
  mFlatShader->Activate();
  mFlatShader->SetMat4("uMatrix", mat);

  fbo->Bind();
  glViewport(0, 0, cam->GetResolution().x, cam->GetResolution().y);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (*viewMode == ViewMode::Color && bgQuad->GetTexture()) {
    glDisable(GL_DEPTH_TEST);
    mQuadShader->Activate();
    bgQuad->GetTexture()->Bind();
    mQuadShader->SetFloat("uDim", 0.0f);
    bgQuad->Draw();
    glEnable(GL_DEPTH_TEST);
  }
}
void Renderer::RenderModel(Camera *cam, FBO *fbo, ViewMode *viewMode,
                           Model *model, glm::vec3 &color) {
  if (!cam || !fbo) return;

  if (*viewMode == ViewMode::Color) {
    mRgbShader->Activate();
    model->Draw(*mRgbShader, *cam, {});
  } else if (*viewMode == ViewMode::Segmentation) {
    mFlatShader->Activate();
    model->Draw(*mFlatShader, *cam, color);
  }
}
void Renderer::End(Quad *dimQuad, float dim, ViewMode *viewMode, FBO *fbo) {
  if (!dimQuad || !fbo || !viewMode || !fbo->ColorTexture) return;

  if (*viewMode == ViewMode::Color) {
    glm::vec2 size = fbo->ColorTexture->GetSize();

    if (mPostProcessFBO->ColorTexture->GetSize() != size) {
      mPostProcessFBO->Resize(size.x, size.y);
    }

    mPostProcessFBO->Bind();
    glDisable(GL_DEPTH_TEST);
    mQuadShader->Activate();
    fbo->ColorTexture->Bind();
    mQuadShader->SetFloat("uDim", dim);
    dimQuad->Draw();
    fbo->ColorTexture->Unbind();
    mPostProcessFBO->Unbind();
    glEnable(GL_DEPTH_TEST);

    mPostProcessFBO->BindRead();
    fbo->BindDraw();

    glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    fbo->Unbind();
    mPostProcessFBO->Unbind();
  }
  fbo->Unbind();
}
