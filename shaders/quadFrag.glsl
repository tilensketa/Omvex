#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uBackgroundTexture;
uniform float RandomDim;

void main()
{
    vec4 color = texture(uBackgroundTexture, vTexCoord);

    vec3 result = vec3(1);
    result *= color.rgb;
    result = max(result * (1.0 - RandomDim), vec3(0));
    FragColor = vec4(result, 1.0f);
}
