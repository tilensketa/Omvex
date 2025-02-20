#version 330 core

out vec4 FragColor;

in vec3 fragColor;
in vec3 fragNormal;
in vec3 fragPos;

vec3 lightPos = vec3(100,100,100);
vec3 lightColor = vec3(1,1,1);

void main()
{
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);

    // Diffuse lighting (Lambertian reflection)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Ambient lighting (small base light)
    vec3 ambient = 0.4 * lightColor;

    // Final color
    vec3 result = (ambient + diffuse) * fragColor;
    FragColor = vec4(result, 1.0);
}
