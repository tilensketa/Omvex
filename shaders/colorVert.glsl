#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

uniform mat4 uCamMatrix;
uniform mat4 uModel;
uniform mat4 uMatrix;

out vec3 fragColor;
out vec3 fragNormal;
out vec3 fragPos;
out vec2 texCoords;

void main()
{
    fragNormal = mat3(transpose(inverse(uModel))) * aNormal;
    fragPos = vec3(uModel * vec4(aPos, 1.0));
    texCoords = aTex;

    gl_Position = uCamMatrix * uMatrix * uModel * vec4(aPos, 1.0f);
}
