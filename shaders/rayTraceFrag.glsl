#version 330 core

out vec4 FragColor;

void main()
{
  vec2 uv = gl_FragCoord.xy;
  if( uv.x > 100){
    discard;
  }
  else{
    FragColor = vec4(1);
  }
}
