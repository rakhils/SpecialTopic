#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;
in vec4 passColor;
in vec2 passUV;
in vec3 passNormal;
in vec3 passWorldPos;
in vec3 lightPosition;
in vec3 cameraPosition;
in vec4 ambientLight;
in float specAmount;
in float specPower;
in float ambientLightEnabled; 
in float dot3LightEnabled;
in float phongEnabled;
in float blinnphongEnabled;
in vec3 passLightPosition[8];
in vec3 passLightColor[8];

out vec4 outColor;

void main( void )
{
int numoflights = 1;

vec4 diffuse = texture( gTexDiffuse, passUV );

vec3 surfaceNormal = passNormal;
vec3 surfaceLight = vec3(0,0,0);

#if defined(AMBIENT) 
      surfaceLight = vec3(ambientLight.x,ambientLight.y,ambientLight.z)*ambientLight.w;
#endif

for(int i = 0;i < 8;i++)
{
float k = passLightColor[i].x*passLightPosition[i].x;
}

vec3 light_dir = lightPosition - passWorldPos;
light_dir = normalize(light_dir);

vec3 eye_dir   = cameraPosition - passWorldPos; 
eye_dir = normalize(eye_dir);
// DOT3
float spec_factor = 0.0f;
if(dot3LightEnabled == 1)
{
	float dot3      = dot(light_dir, surfaceNormal); 
	vec3 lightColor = max(dot3, 0.0f) * ambientLight.xyz; 
	surfaceLight    += lightColor; 
}

//PHONG
vec3 reflected_light = vec3(0);

if(phongEnabled == 1)
{
	vec3 reflectVector = reflect( -light_dir, surfaceNormal ); 
	spec_factor        = max( 0, dot( eye_dir, reflectVector ) ); 
}
if(blinnphongEnabled == 1)
{
	vec3 halfvector = normalize(eye_dir + light_dir);
	spec_factor     = max( 0, dot( surfaceNormal, halfvector ) ); 
}

spec_factor = (specAmount)*pow( spec_factor, (specPower) ); 

reflected_light = ambientLight.xyz * spec_factor;
reflected_light = clamp(reflected_light,vec3(0),vec3(1));


vec4 final_color = vec4(surfaceLight, 1) + vec4(reflected_light, 1);
outColor = final_color*vec4(passColor)*diffuse*(passLightPosition[0].x + 1 + passLightColor[0].x);


//vec3 surfaceNormalColor = (surfaceNormal + vec3(1)) * .5f;
//surfaceNormal = (surfaceNormal +1)/2;
//outColor = vec4( surfaceNormalColor, 1 ); 

}

vec3 ComputeDot3(in vec3 lightposition,in vec3 lightcolor,in vec3 worldposition,in vec3 worldnormal)
{
	vec3 lightDirection = lightposition - worldposition;
	float dot3      = dot(lightDirection, worldnormal); 
	vec3 lightColor = max(dot3, 0.0f) * lightcolor.xyz; 
	return lightColor; 
}