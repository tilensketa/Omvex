#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uCamMatrix;
uniform mat4 uModel;
uniform mat4 uMatrix;

void main()
{
  gl_Position = uCamMatrix * uMatrix * uModel * vec4(aPos, 1.0f);
}
