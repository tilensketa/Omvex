#pragma once

#include <glad/glad.h>
#include <iostream>
#include "Shader.h"

class Quad {
public:
    Quad();
    ~Quad();

    void Draw(Shader& shader, bool drawTexture);
    void SetTexture(unsigned int textureID);
    
private:
    unsigned int VAO, VBO;
    unsigned int texture;
};
