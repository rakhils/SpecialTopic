#pragma once
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Component.hpp"
/*\class  : Light	   
* \group  : <GroupName>
* \brief  :	   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman	   
* \version: 1.0
* \date   : 4/8/2018 2:47:06 PM
* \contact: srsrakhil@gmail.com
*/
class Renderer;
class UniformBuffer;
const int MAX_LIGHTS = 16;

struct PointLight_t
{
	Vector3 m_position;
	float   pad00;

	Vector3 m_direction;
	float   m_directionFactor = 1;

	Vector4 m_colorIntensity;

	Vector3 m_attenuation;
	float   m_dotInnerAngle;

	Vector3 m_specAttenuation; 
	float   m_dotOuterAngle;

	PointLight_t()
	{
		Reset();
	}

	void Reset()
	{
		m_position	      = Vector3(0, 0, 0);
		m_direction		  = Vector3(0, 0, 0);
		m_colorIntensity  = Vector4(0, 0, 0, 0);
		m_attenuation     = Vector3(1, 0, 0);
	
		m_specAttenuation = Vector3(1, 0, 0);

		m_dotInnerAngle = 1;// 0.965f;  // 15 degree
		m_dotOuterAngle = 0;// 0.707f;  //45 Degree
	}

	void SetupForPointLight(Vector4 colorIntensity, Vector3 position, Vector3 attenutaionConstants)
	{
		m_colorIntensity = colorIntensity;
		m_position = position;
		m_attenuation = attenutaionConstants;
	}

	void SetAsPointLight()
	{
		m_dotInnerAngle = 3.14f;
		m_dotOuterAngle = 3.14f;
	}

	void SetInnerAndOuterAngles(float innerAngle, float outerAngle);

	void DisablePointLight() { m_colorIntensity.w = 0.0f; }

	bool IsLightDisabled()
	{
		if (m_colorIntensity.w == 0.0f || m_colorIntensity.XYZ() == Vector3(0, 0, 0))
		{
			return true;
		}
		return false;
	}
};

struct AmbientLightStruct
{
	Vector4 m_ambientColor;
	AmbientLightStruct()
	{
		Reset();
	}

	void Reset()
	{
		m_ambientColor = Vector4(0, 0, 0, 0);
	}
	void SetAmbientLightColor(Vector4 color)
	{
		m_ambientColor = color;
	}
	Vector4 GetAmbientLightColor()
	{
		return m_ambientColor;
	}
};

/*
struct PointLightFactorObj_t
{
	float m_specularFactor = 1.f;
	float m_specularPower  = 1.f;
	Vector2 m_pad00;
};
*/
enum LIGHT_TYPE
{
	AMBIENT_LIGHT,
	POINT_LIGHT,
};

class Light : public Component
{

public:
	//Member_Variables

	AmbientLightStruct			m_ambientLight;
	PointLight_t				m_pointLightObj;

	Renderer*					m_renderer = nullptr;
	bool						m_isPointLight;
	static UniformBuffer*		m_PointlightUBuffer;
	static UniformBuffer*		m_AmbientlightUBuffer;
	int							m_index;
	LIGHT_TYPE					m_type;
	//Static_Member_Variables
	static std::vector<Light*>  s_lights;
	//Methods

	Light(Renderer *renderer);
	Light();
	~Light();
	void			Reset();
	void			EnableAmbientLight(Vector4 ambient);
	void			DisableAmbientLight();

	bool			IsPointLightActive();
	bool			IsAmbientLightActive();
	void			EnablePointLight(Vector4 colorIntensity, Vector3 position, Vector3 attenuationConst);
	void			DisablePointLight();

	float			CalculateIntensityOnAttenutation(float distance, Vector3 attenuationFactor);

	Vector3			GetPointLightPosition();
	Vector3			GetPointLightDirection();
	Vector3			GetPointLightAttenuationFactor();
	Rgba			GetPointLightColor();
	float			GetPointLightSpecFactor();
	float			GetPointLightSpecPower();
	float			GetPointLightInnerAngle();
	float			GetPointLightOuterAngle();

	void			SetAsPointLight();
	void			SetPointLightDirection(Vector3 direction);
	void			SetPointLightInnerOuterAngles(float innerAngle, float outerAngle);
	void			SetPointLightPosition(Vector3 position);
	void			SetPointLightAttenutaion(Vector3 attenuation);
	void			SetPointLigthSpecAttenuation(Vector3 specAttenuation);
	void			SetPointLightColor(Rgba color);
	void			SetSpecularConstants(float specPower, float specStrength);

	void			Update(float deltaTime);
	void			BindAllShaderValues();

	//Static_Methods
	static void    Initialize();
	static Light*  CreateOrGetPointLight();
	static Light*  CreateAmbientLight();
	static int	   GetFreeLightIndex();
	static void	   DestroyLight(int index);
	static void    BindAllLightsToShader(Renderer *renderer);
	static void    BindAllLightsToShader(Renderer *renderer, std::vector<Light*> lights);

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};