#version 330 core

out vec4 FragColor;

in vec3 fragColor;
in vec3 fragNormal;
in vec3 fragPos;

in vec2 texCoords;

uniform bool HasTexture;
uniform bool IsShaded;
uniform sampler2D tex1;

vec3 lightPos = vec3(100,100,100);
vec3 lightColor = vec3(1,1,1);

void main()
{
  vec3 result = vec3(1,1,1);
  if(IsShaded){
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 ambient = 0.4 * lightColor;
    result = (ambient + diffuse);
  }

  vec4 texColor = texture(tex1, texCoords);
  if(HasTexture){
    result *= texColor.rgb;
  }
  else{
    result *= fragColor;
  }
  FragColor = vec4(result, 1.0f);
}
