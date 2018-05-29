#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Light*> Light::s_lights;
UniformBuffer*		Light::m_PointlightUBuffer   = nullptr;
UniformBuffer*		Light::m_AmbientlightUBuffer = nullptr;
// CONSTRUCTOR
Light::Light()
{
	
}

// CONSTRUCTOR
Light::Light(Renderer *renderer)
{
	m_renderer = renderer;
}

// DESTRUCTOR
Light::~Light()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/03
*@purpose : Reset all lights
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::Reset()
{
	m_ambientLight.Reset();
	m_pointLightObj.Reset();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : Enables currentLight as ambient light
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::EnableAmbientLight(Vector4 ambientLightColor)
{
	m_ambientLight.SetAmbientLightColor(ambientLightColor);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : Disables the current ambient light
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::DisableAmbientLight()
{
	m_ambientLight.SetAmbientLightColor(Vector4(0,0,0,0));
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : Checks if light is active
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
bool Light::IsPointLightActive()
{
	if(m_pointLightObj.IsLightDisabled())
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : Check if this light is ambient type
*@param   : NIL
*@return  : True if ambient else false
*//////////////////////////////////////////////////////////////
bool Light::IsAmbientLightActive()
{
	if(m_ambientLight.m_ambientColor == Vector4(0,0,0,0))
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/16
*@purpose : Enables current ligth as point light
*@param   : Color intensity,position,attenuation const
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::EnablePointLight(Vector4 colorIntensity,Vector3 position,Vector3 attenuationConst)
{
	m_pointLightObj.SetupForPointLight(colorIntensity, position, attenuationConst);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : Disables the point light
*@param   : NIL
*@return  : NIL
*/////////////////////////////////////////////////////////////
void Light::DisablePointLight()
{
	m_pointLightObj.DisablePointLight();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/01
*@purpose : Calculate intensity with distance and attenuation factor
*@param   : Distance from source,Attentuation factor
*@return  : NIL
*//////////////////////////////////////////////////////////////
float Light::CalculateIntensityOnAttenutation(float distance,Vector3 attenuationFactor)
{
	return m_pointLightObj.m_colorIntensity.w / (attenuationFactor.x + distance * attenuationFactor.y + distance * distance * attenuationFactor.z);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : Get point light position
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
Vector3 Light::GetPointLightPosition()
{
	return m_pointLightObj.m_position;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/25
*@purpose : Get point ligth direction
*@param   : NIL
*@return  : Direction unit vector
*//////////////////////////////////////////////////////////////
Vector3 Light::GetPointLightDirection()
{
	return m_pointLightObj.m_direction;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/02
*@purpose : Get point light attentuation factor
*@param   : NIL
*@return  : Attentuation factor
*//////////////////////////////////////////////////////////////
Vector3 Light::GetPointLightAttenuationFactor()
{
	return m_pointLightObj.m_attenuation;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/25
*@purpose : Get point ligth color
*@param   : NIL
*@return  : Color
*//////////////////////////////////////////////////////////////
Rgba Light::GetPointLightColor()
{
	return Rgba(m_pointLightObj.m_colorIntensity);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/26
*@purpose : Get spec factor
*@param   : NIL
*@return  : Spec values
*//////////////////////////////////////////////////////////////
float Light::GetPointLightSpecFactor()
{
	return 1;// m_pointLightObj.m_specularFactor;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/26
*@purpose : returns Spec power
*@param   : NIL
*@return  : Spec power
*//////////////////////////////////////////////////////////////
float Light::GetPointLightSpecPower()
{
	return 1;/// m_pointLightObj.m_specularPower;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/26
*@purpose : Get point ligth's inner angle value(angle till intensity 1)
*@param   : NIL
*@return  : Inner angle values 
*/////////////////////////////////////////////////////////////
float Light::GetPointLightInnerAngle()
{
	return m_pointLightObj.m_dotInnerAngle;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/26
*@purpose : Gets point light's outer angle (angle from intensity decreases from 1 to 0)
*@param   : NIL
*@return  : Outer angle value
*//////////////////////////////////////////////////////////////
float Light::GetPointLightOuterAngle()
{
	return m_pointLightObj.m_dotOuterAngle;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/05
*@purpose : Sets as point light
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::SetAsPointLight()
{
	m_pointLightObj.SetAsPointLight();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/23
*@purpose : Sets point light direction
*@param   : Direction
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::SetPointLightDirection(Vector3 direction)
{
	m_pointLightObj.m_direction = direction;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/21
*@purpose : Set point ligth's inner and outer angle
*@param   : Inner and outer angle values
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::SetPointLightInnerOuterAngles(float innerAngle, float outerAngle)
{
	m_pointLightObj.SetInnerAndOuterAngles(innerAngle, outerAngle);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/19
*@purpose : Set point ligth position
*@param   : Position
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::SetPointLightPosition(Vector3 position)
{
	m_pointLightObj.m_position = position;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : Set point light attenuation factor values(xyz)
*@param   : Attenuation factor
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::SetPointLightAttenutaion(Vector3 attentuation)
{
	m_pointLightObj.m_attenuation = attentuation;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/23
*@purpose : Set point light spec attenuation value
*@param   : Spec attenuation factor
*@return  : NIL
*/////////////////////////////////////////////////////////////
void Light::SetPointLigthSpecAttenuation( Vector3 specAttenuation)
{
	m_pointLightObj.m_specAttenuation = specAttenuation;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : Set point ligth color
*@param   : Color
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::SetPointLightColor(Rgba color)
{
	m_pointLightObj.m_colorIntensity = color.GetAsFloats();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : Set spec const values
*@param   : Spec power,spec factor
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::SetSpecularConstants(float specPower, float specFactor)
{
	UNUSED(specFactor);
	UNUSED(specPower);
	//m_pointLightObj.m_specularPower = specPower;
	//m_pointLightObj.m_specularFactor = specFactor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Updates component factor of light
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Light::Update(float deltaTime)
{
	UNUSED(deltaTime);
	m_pointLightObj.m_position = m_transform.GetWorldPosition();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : Bind all ligth values to current shader
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::BindAllShaderValues()
{
	//m_renderer->BindUniformBuffer(5, m_lightUBuffer, sizeof(PointLights_t), &pointLightObj);
	//m_renderer->BindUniformBuffer(6, m_lightFactorUBuffer, sizeof(PointLightFactorObj_t), &pointLightFactorObj);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/02
*@purpose : Init ligths( sets color as black (reset))
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::Initialize()
{
	m_PointlightUBuffer   = new UniformBuffer();
	m_AmbientlightUBuffer = new UniformBuffer(); 
	for (int index = 0; index < MAX_LIGHTS; index++)
	{
		Light* light = new Light();
		light->Reset();
		light->m_type = POINT_LIGHT;
		s_lights.push_back(light);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/02
*@purpose : Create a new ligth object and push to static container
*@param   : NIL
*@return  : Light class pointer
*//////////////////////////////////////////////////////////////
Light* Light::CreateOrGetPointLight()
{
	int index = GetFreeLightIndex();
	if (index == -1)
	{
		return nullptr;
	}
	Light *light = s_lights.at(index);
	light->Reset();
	light->m_type = POINT_LIGHT;
	return light;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/03
*@purpose : Creates a new ambient light and pushes to static container
*@param   : NIL
*@return  : Light pointer
*//////////////////////////////////////////////////////////////
Light* Light::CreateAmbientLight()
{
	int index = GetFreeLightIndex();
	if(index == -1)
	{
		return nullptr;
	}
	Light *light = s_lights.at(index);
	light->Reset();
	light->m_type = AMBIENT_LIGHT;
	return light;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/02
*@purpose : Get free index in ligth array 
*@param   : NIL
*@return  : free Index 
*/////////////////////////////////////////////////////////////
int Light::GetFreeLightIndex()
{
	for (int index = 0; index < s_lights.size(); index++)
	{
		if(!s_lights.at(index)->IsPointLightActive() && !s_lights.at(index)->IsAmbientLightActive())
		{
			return index;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/01
*@purpose : Bind all ligth values to shader
*@param   : Renderer pointer
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::BindAllLightsToShader(Renderer *renderer)
{
	PointLight_t lights[MAX_LIGHTS];
	AmbientLightStruct ambientLight;
	int pointLightIndex = 0;
	for(int index = 0;index < s_lights.size(); index++)
	{
		switch (s_lights.at(index)->m_type)
		{
		case POINT_LIGHT:
			lights[pointLightIndex++] = s_lights.at(index)->m_pointLightObj;
			break;
		case AMBIENT_LIGHT:
			ambientLight = s_lights.at(index)->m_ambientLight;
			break;
		default:
			break;
		}
	}
	renderer->SetUniform("CAMERA_POSITION", Camera::GetCurrentCamera()->m_transform.GetLocalPosition());
	

	renderer->BindUniformBuffer(5, m_PointlightUBuffer  , sizeof(PointLight_t)*(MAX_LIGHTS), lights);
	renderer->BindUniformBuffer(6, m_AmbientlightUBuffer, sizeof(AmbientLightStruct), &ambientLight);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/10
*@purpose : Bind all given light values to shader 
*@param   : Renderer pointer and ligths to be binded
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Light::BindAllLightsToShader(Renderer *renderer, std::vector<Light*> lights)
{
	PointLight_t pLights[MAX_LIGHTS];
	AmbientLightStruct ambientLight;
	int pointLightIndex = 0;
	for (int index = 0; index < lights.size(); index++)
	{
		switch (lights.at(index)->m_type)
		{
		case POINT_LIGHT:
			pLights[pointLightIndex++] = lights.at(index)->m_pointLightObj;
			break;
		case AMBIENT_LIGHT:
			ambientLight = lights.at(index)->m_ambientLight;
			break;
		default:
			break;
		}
	}
	renderer->SetUniform("CAMERA_POSITION", Camera::GetCurrentCamera()->m_transform.GetLocalPosition());


	renderer->BindUniformBuffer(5, m_PointlightUBuffer, sizeof(PointLight_t)*(MAX_LIGHTS), pLights);
	renderer->BindUniformBuffer(6, m_AmbientlightUBuffer, sizeof(AmbientLightStruct), &ambientLight);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/21
*@purpose : Sets inner and outer angles of point light
*@param   : Inner and outer angle
*@return  : NIL
*//////////////////////////////////////////////////////////////
void PointLight_t::SetInnerAndOuterAngles(float innerAngle, float outerAngle)
{
	//innerAngle = 30;
	//outerAngle = 90;
	m_dotInnerAngle = CosDegrees(innerAngle);
	m_dotOuterAngle = CosDegrees(outerAngle);
}
