#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uBackgroundTexture;

void main()
{
    vec4 color = texture(uBackgroundTexture, vTexCoord);
    FragColor = color;
}
