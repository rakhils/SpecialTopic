#version 420 core
uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;
uniform vec3 CAMERA_POSITION;
uniform float SPEC_FACTOR;
uniform float SPEC_POWER;

in vec2 UV;
in vec3 POSITION;
in vec4 COLOR;
in vec3 NORMAL;
in vec3 TANGENT;
in vec3 BITANGENT;

out vec2 passUV;
out vec4 passColor;
out vec3 passNormal;
out vec3 passWorldPos;
out vec3 passTangent;
out vec3 passBitangent;
out vec3 cameraPosition;
out vec3 passViewPos;
out vec3 passWorld;

out float passSpecFactor;
out float passSpecPower;
void main( void )
{
vec4 local_pos= vec4(POSITION,1);
vec4 clip_pos = PROJECTION*VIEW*MODEL*local_pos;

vec4 world_pos  = MODEL * local_pos;
passWorld = world_pos.xyz;
vec4 camera_pos = VIEW * world_pos; 
passViewPos = camera_pos.xyz; 

passWorldPos = POSITION;
passColor = COLOR;
passUV = UV;
passNormal = NORMAL;
cameraPosition = CAMERA_POSITION;
passBitangent = BITANGENT;
passTangent = TANGENT;
passSpecFactor = SPEC_FACTOR;
passSpecPower  = SPEC_POWER;

gl_Position = clip_pos;
}