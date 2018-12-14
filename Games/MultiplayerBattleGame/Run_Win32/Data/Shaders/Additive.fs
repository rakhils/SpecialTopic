#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;
in vec4 passColor;
in vec2 passUV;
in vec4 passModelColor;

out vec4 outColor;
void main( void )
{
	vec4 tex_color = texture( gTexDiffuse, passUV ); 
    vec4 final_color = tex_color * passColor;

   if (final_color.a <= .5f)
   {
      discard; 
   }

   outColor = final_color; 
}
