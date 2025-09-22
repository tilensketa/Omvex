#include "Rendering/Buffers/FBO.h"

#include "Core/Logger.h"

FBO::FBO(int width, int height) {
  glGenFramebuffers(1, &ID);
  recreateFramebuffer(width, height);
}

void FBO::Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, ID); }
void FBO::Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void FBO::BindRead() const { glBindFramebuffer(GL_READ_FRAMEBUFFER, ID); }
void FBO::BindDraw() const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ID); }
void FBO::Delete() {
  if (DepthStencilID != 0) glDeleteRenderbuffers(1, &DepthStencilID);
  if (ID != 0) glDeleteFramebuffers(1, &ID);
}

void FBO::Resize(int newWidth, int newHeight) {
  recreateFramebuffer(newWidth, newHeight);
}

void FBO::recreateFramebuffer(int width, int height) {
  Bind();
  createColorAttachment(width, height);
  createDepthStencilAttachment(width, height);
  checkComplete();
  Unbind();
}
void FBO::createColorAttachment(int width, int height) {
  ColorTexture = std::make_unique<Texture>(width, height);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         ColorTexture->GetTextureID(), 0);
}
void FBO::createDepthStencilAttachment(int width, int height) {
  if (DepthStencilID != 0) {
    glDeleteRenderbuffers(1, &DepthStencilID);
  }
  glGenRenderbuffers(1, &DepthStencilID);
  glBindRenderbuffer(GL_RENDERBUFFER, DepthStencilID);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, DepthStencilID);
}
void FBO::checkComplete() const {
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    Logger::Error("Framebuffer not complete");
    throw ::std::runtime_error("Framebuffer setup failed");
  }
}
