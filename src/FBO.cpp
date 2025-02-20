#include "FBO.h"

// Constructor that generates a Elements Buffer Object and links it to indices
FBO::FBO(int width, int height) {
  glGenFramebuffers(1, &ID);
  glBindFramebuffer(GL_FRAMEBUFFER, ID);

  // Create texture for color attachment
  colorTexture = std::make_unique<Texture>(width, height);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         colorTexture->GetTextureID(), 0);

  // Create renderbuffer for depth and stencil
  glGenRenderbuffers(1, &depthStencilBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, depthStencilBuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer not complete");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::Bind() { glBindFramebuffer(GL_FRAMEBUFFER, ID); }
void FBO::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void FBO::Delete() {
  glDeleteFramebuffers(1, &ID);
  glDeleteRenderbuffers(1, &depthStencilBuffer);
}
