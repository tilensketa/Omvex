#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 matrix;

out vec3 fragColor;
out vec3 fragNormal;
out vec3 fragPos;
out vec2 texCoords;

void main()
{
    fragColor = aColor;
    fragNormal = mat3(transpose(inverse(model))) * aNormal;
    fragPos = vec3(model * vec4(aPos, 1.0));
    texCoords = aTex;

    gl_Position = camMatrix * matrix * model * vec4(aPos, 1.0f);
}
