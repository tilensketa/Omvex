#version 330 core

out vec4 FragColor;

uniform vec3 uniqueColor;

void main()
{
  FragColor = vec4(uniqueColor,1);
}
