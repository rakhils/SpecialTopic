#version 420 core
uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;
uniform vec3 CAMERA_POSITION;
uniform vec3 LIGHT_POSITION;
uniform vec4 AMBIENT_LIGHT;
uniform float SPEC_POWER;
uniform float SPEC_AMOUNT;
uniform float AMBIENT_LIGHT_ENABLED;
uniform float DOT3_LIGHT_ENABLED;
uniform float PHONG_ENABLED;
uniform float BLINNPHONG_ENABLED;
uniform vec3 PLIGHT_POSITION[8];
uniform vec3 PLIGHT_COLOR[8];

in vec2 UV;
in vec3 POSITION;
in vec4 COLOR;
in vec3 NORMAL;

out vec2 passUV;
out vec4 passColor;
out vec3 passNormal;
out vec3 passWorldPos;
out vec3 cameraPosition;
out vec3 lightPosition;
out float specPower;
out float specAmount;
out vec4 ambientLight;
out float ambientLightEnabled;
out float dot3LightEnabled;
out float phongEnabled;
out float blinnphongEnabled;
out vec3 passLightPosition[8];
out vec3 passLightColor[8];

void main( void )
{
vec4 local_pos= vec4(POSITION,1);
vec4 clip_pos = PROJECTION*VIEW*MODEL*local_pos;
passWorldPos = local_pos.xyz;
passColor=COLOR;
passUV = UV;
passNormal = NORMAL;
cameraPosition = CAMERA_POSITION;
lightPosition  = LIGHT_POSITION;
specPower = SPEC_POWER;
specAmount = SPEC_AMOUNT;
ambientLight = AMBIENT_LIGHT;
ambientLightEnabled = AMBIENT_LIGHT_ENABLED;
dot3LightEnabled = DOT3_LIGHT_ENABLED;
phongEnabled = PHONG_ENABLED;
blinnphongEnabled = BLINNPHONG_ENABLED;
int lightindex = 0;
for(;lightindex < 8;lightindex++)
{
passLightPosition[lightindex] = PLIGHT_POSITION[lightindex];
passLightColor[lightindex]    = PLIGHT_COLOR[lightindex];
}
gl_Position = clip_pos;
}