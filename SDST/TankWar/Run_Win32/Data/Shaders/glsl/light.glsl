struct light_point 
{
	vec3	position;
	float   uses_shadow; 

	vec3	forward;
	float   directionFactor;

	vec4	color;

	vec3	attenuation;
	float   dotInnerAngle;

	vec3	specAttenuation;
	float   dotOuterAngle;
 
	mat4 shadow_vp;
}; 

struct Fog
{
	float FOG_NEAR_PLANE;
    float FOG_FAR_PLANE;
// float FOG_NEAR_FACTOR;
// float FOG_FAR_FACTOR;
}

struct light_factor
{
vec3 diffuse;
vec3 ambient;
};

struct light_ambient
{
	vec4 color;
};

layout (binding=6, std140) uniform cAmbientBlock
{
	light_ambient AMBIENT_LIGHT;
};

layout (binding=5, std140) uniform cLightBlock
{
	light_point POINT_LIGHTS[8];
};
layout (binding=7, std140) uniform cFogBlock
{
	Fog fogFactor;
};

vec4 AddFog(vec4 color,float depth)
{
	float fog_factor = smoothstep( FOG_NEAR_PLANE, FOG_FAR_PLANE, depth ); 
    fog_factor = 0 + (1 - 0) * fog_factor; 
    color = mix( color, vec4(1,1,1,0), fog_factor ); 
	return color;
}


