#version 420 core


layout(binding = 8) uniform samplerCube gTexSky;

in vec3 passWorldPosition; 
in vec4 passColor; 

out vec4 outColor; 

void main( void )
{
  vec3 normal = normalize(passWorldPosition); 
  vec4 tex_color = texture( gTexSky, normal ); 

  //vec4 diffuse = texture( gTexDiffuse, passUV );
outColor = tex_color*passColor;

//outColor = vec4(normal,1);
}
