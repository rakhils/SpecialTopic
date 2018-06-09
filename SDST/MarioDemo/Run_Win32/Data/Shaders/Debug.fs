#version 420 core
#include "glsl/light.glsl" 

layout(binding = 0) uniform sampler2D gTexDiffuse;
in vec4 passColor;
in vec2 passUV;
in vec4 passModelColor;
out vec4 outColor;
void main( void )
{
vec4 diffuse = texture( gTexDiffuse, passUV );
outColor = diffuse*passColor*passModelColor;
}
