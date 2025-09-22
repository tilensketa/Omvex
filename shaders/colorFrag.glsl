#version 460 core

out vec4 FragColor;

in vec3 fragColor;
in vec3 fragNormal;
in vec3 fragPos;

in vec2 texCoords;

uniform bool uHasTexture;
uniform sampler2D uTex1;

vec3 lightPos = vec3(100,100,100);
vec3 lightColor = vec3(1,1,1);

void main()
{
  vec3 norm = normalize(fragNormal);
  vec3 lightDir = normalize(lightPos - fragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;
  vec3 ambient = 0.4 * lightColor;
  vec3 result = (ambient + diffuse);

  vec4 texColor = texture(uTex1, texCoords);
  if(uHasTexture){
    result *= texColor.rgb;
  }
  else{
    result *= fragColor;
  }

  FragColor = vec4(result, 1.0f);
}
