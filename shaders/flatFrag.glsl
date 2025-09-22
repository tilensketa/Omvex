#version 460 core

out vec4 FragColor;

uniform vec3 uUniqueColor;

void main()
{
  FragColor = vec4(uUniqueColor, 1.0f);
}
