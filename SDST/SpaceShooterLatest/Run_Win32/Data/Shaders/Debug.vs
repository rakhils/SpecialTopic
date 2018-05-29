#version 420 core
uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;
uniform vec4 MODELCOLOR;

in vec2 UV;
in vec3 POSITION;
in vec4 COLOR;
out vec2 passUV;
out vec4 passColor;
out vec4 passModelColor;
void main( void )
{
vec4 local_pos= vec4(POSITION,1);
vec4 clip_pos = PROJECTION*VIEW*MODEL*local_pos;
//vec4 clip_pos = local_pos*MODEL*VIEW*PROJECTION;
passColor=COLOR;
passUV = UV;
passModelColor = MODELCOLOR;
gl_Position = clip_pos;
}