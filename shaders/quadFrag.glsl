#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uBackgroundTexture;

void main()
{
    // Sample the background texture
    vec4 color = texture(uBackgroundTexture, vTexCoord);

    // Example post-processing: Convert to grayscale
    // float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    // color = vec4(vec3(color.rgb), color.a);

    FragColor = color;
}
