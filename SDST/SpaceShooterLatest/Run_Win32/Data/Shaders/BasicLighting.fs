#version 420 core
#include "glsl/light.glsl" 

layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal;
in vec4 passColor;
in vec2 passUV;
in vec3 passNormal;
in vec3 passWorldPos;
in vec3 passTangent;
in vec3 passBitangent;
in vec3 cameraPosition;
in float passSpecPower;
in float passSpecFactor;

out vec4 outColor;

float GetAttenuation( float intensity, float distance, vec3 attenuationfactor )
{
   return intensity / (attenuationfactor.x + distance * attenuationfactor.y + distance * distance * attenuationfactor.z);
}
vec3 WorldNormalToColor( vec3 normal )
{
   return (normal + vec3(1.0f)) * .5f; 
}

vec3 ColorToSurfaceNormal( vec3 color ) 
{
   return normalize( (vec3(2.0f, 2.0f, 1.0f) * color) - vec3( 1.0f, 1.0f, 0.0f ) ); 
}

vec3 ComputeDot3(light_point light,in vec3 worldposition,in vec3 worldnormal)
{
	float distanceFromSource	= length(light.position - worldposition);
	vec3  lightDirection		= normalize(light.position - worldposition);

	//lightDirection			    = mix(lightDirection, -light.forward, light.directionFactor);
	float attenuation			= GetAttenuation(light.color.w,distanceFromSource,light.attenuation);
	
	float dot3					= attenuation*dot(lightDirection, worldnormal); 
	vec3 diffuselightColor		= max(dot3, 0.0f) * light.color.xyz; 
	return diffuselightColor; 
}

vec3 ComputeSpecular(light_point light,in vec3 eyeDirection,in vec3 worldPosition,in vec3 worldnormal)
{
	vec3  lightDirection        = light.position - worldPosition;
	float distanceFromSource	= length(lightDirection);
	lightDirection				= lightDirection/distanceFromSource;
	vec3  lightForward		    = normalize(light.forward); 
	float lightPower			= light.color.w;
	float dotLightangle			= dot(lightForward,-lightDirection);
	float angleAttenuation		= smoothstep(light.dotOuterAngle, light.dotInnerAngle, dotLightangle );
	
	lightPower					= lightPower * angleAttenuation;
	
	//lightDirection			    = mix(lightDirection, -lightForward,1);// light.directionFactor);
	
	float specAttenuation	    = clamp( GetAttenuation( lightPower, distanceFromSource, light.specAttenuation ), 0, 1 );
	//specAttenuation = 1;
	vec3 reflectVector			= reflect( -lightDirection, worldnormal ); 
	float specAmount			= max(dot(reflectVector, lightDirection), 0.0f);	
	float specIntensity         = (specAttenuation *passSpecFactor) * pow(specAmount, passSpecPower); 
	vec3 specularLight		    = light.color.xyz * specIntensity;

	return specularLight;
}
void main( void )
{


vec4 surfaceColor   = texture( gTexDiffuse, passUV );
vec3 normalcolor    = texture( gTexNormal,  passUV ).xyz;

vec3 worldtangent   = normalize(passTangent); 
vec3 worldbitangent = normalize(passBitangent);
vec3 worldnormal    = normalize(passNormal);

mat3 surfaceToWorldMatrix = transpose( mat3( worldtangent, worldbitangent, worldnormal ) ); 

vec3 surfaceNormal = normalize( normalcolor * vec3( 2.0f, 2.0f, 1.0f ) + vec3( -1.0f, -1.0f, 0.0f ) ); 
vec3 worldNormal   = surfaceToWorldMatrix * surfaceNormal ; 

vec3 diffuseLight = AMBIENT_LIGHT.color.xyz*AMBIENT_LIGHT.color.w;
vec3 specularLight = vec3(0);

vec3 eyeDirection   = cameraPosition - passWorldPos; 
eyeDirection = normalize(eyeDirection);

vec3 normalToBeUsed = vec3(0);

//if(passIsUsingNormalMap == 1)
{
	normalToBeUsed = worldNormal;
}
//else
{	
	normalToBeUsed = passNormal;
}

for(int lightIndex = 0;lightIndex < 16;lightIndex++)
{
	diffuseLight  += ComputeDot3(POINT_LIGHTS[lightIndex],passWorldPos,normalize(normalToBeUsed));
	specularLight += ComputeSpecular(POINT_LIGHTS[lightIndex],eyeDirection,passWorldPos,normalize(normalToBeUsed));
}

//specularLight   = clamp(specularLight,vec3(0),vec3(1));
diffuseLight     = clamp(diffuseLight,vec3(0),vec3(1));

vec4 final_color = vec4(diffuseLight, 1)*surfaceColor + vec4(specularLight, 1);
final_color		 = clamp(final_color,vec4(0),vec4(1));
outColor	     = final_color;//vec4(passSpecFactor/10,passSpecPower/16,0,1);
//outColor		 = vec4(normalize(POINT_LIGHTS[0].position),1);
//switch (int(passRenderMode))vrea
//{
//      case 1: outColor  = vec4( passUV, 0, 1.0f ); break;
//      case 2: outColor  = vec4( WorldNormalToColor( normalize(passNormal) ), 1);  break;
//      case 3: outColor  = vec4( WorldNormalToColor( normalize(worldtangent) ), 1); break;
//      case 4: outColor  = vec4( WorldNormalToColor( normalize(worldbitangent) ), 1);  break;
//      case 5: outColor  = vec4( normalcolor, 1 ); break;
//      case 6: outColor  = vec4( WorldNormalToColor( normalize(worldNormal) ), 1); break;
//	  case 7: outColor  = surfaceColor;  break;
//	  case 8: outColor  = vec4(diffuseLight, 1); break;
//	  case 9: outColor  = vec4(specularLight, 1)*surfaceColor; break;
//	  case 10: outColor = final_color; break;
//      default: 
//		//if (RENDER_MIX == vec4(1,0,0,0)) 
//		//{
//        // outColor = tex_color; 
//		//} 
//		break; 
//}
//outColor = AMBIENT_LIGHT.color;
//outColor = outColor*passTint;
//outColor = vec4(LIGHTS[0].attenuation.xyz,1);
//outColor = vec4( normalcolor, 1 );

//vec3 surfaceNormalColor = (passNormal + vec3(1)) * .5f;
//outColor = vec4(surfaceNormalColor,1);
//passNormal = (passNormal +1)/2;
//outColor = vec4( surfaceNormalColor, 1 ); 
//outColor = vec4(specularLight.x,0,0,1);
}

