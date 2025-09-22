#version 460 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform float uDim;

uniform sampler2D uBackgroundTexture;

void main()
{
  vec4 color = texture(uBackgroundTexture, vTexCoord);
  color.rgb *= (1.0 - uDim);
  color.a = 1.0;
  FragColor = color;
}
