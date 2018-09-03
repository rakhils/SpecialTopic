struct light_point 
{
	vec3	position;
	float   pad00;

	vec3	forward;
	float   directionFactor;

	vec4	color;

	vec3	attenuation;
	float   dotInnerAngle;

	vec3	specAttenuation;
	float   dotOuterAngle;
 
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