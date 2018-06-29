#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;
in vec4 passColor;
in vec2 passUV;
in vec4 passModelColor;
in float passTime;

out vec4 outColor;
void main( void )
{
	vec2 uv_offset      =  passUV + vec2(passTime);
	vec4 surfaceColor   = texture( gTexDiffuse, passUV + uv_offset ) * passColor; 
    vec4 final_color = surfaceColor * passColor;

   if (final_color.a <= .5f)
   {
      discard; 
   }

   outColor = final_color; 
}



