#include "Quad.h"

// Quad Vertex Data (Fullscreen Quad)
const float quadVertices[] = {
    // Positions   // Texture Coords
    -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

// Constructor
Quad::Quad() {
  // Generate VAO & VBO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // Bind VAO
  glBindVertexArray(VAO);

  // Bind & Fill VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_STATIC_DRAW);

  // Set Vertex Attribute Pointers
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

// Destructor
Quad::~Quad() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}

// Set Texture
void Quad::SetTexture(unsigned int textureID) { texture = textureID; }

// Render Quad
void Quad::Draw(Shader &shader, bool drawTexture) {
  shader.Activate();
  glBindVertexArray(VAO);

  if (drawTexture) {
    // Bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.SetInt("uBackgroundTexture", 0);
  }

  // Draw Quad
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
}
