#include "Engine/Renderer/Lights/PointLight.hpp"
#include "Engine/Renderer/Renderer.hpp"
//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Sets all unifroms needed for point ligth in current active shader
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void PointLight::SetUniformsInShader()
{
	float VALUEONE  = 1.f;
	float VALUEZERO = 0.f;
	if (m_dot3Enabled)
	{
		m_renderer->SetUniform("DOT3_LIGHT_ENABLED", VALUEONE);
	}
	else
	{
		m_renderer->SetUniform("DOT3_LIGHT_ENABLED", VALUEZERO);
	}

	if (m_phongEnabled)
	{
		m_renderer->SetUniform("PHONG_ENABLED", VALUEONE);
	}
	else
	{
		m_renderer->SetUniform("PHONG_ENABLED", VALUEZERO);
	}

	if (m_BlinnphongEnabled)
	{
		m_renderer->SetUniform("BLINNPHONG_ENABLED", VALUEONE);
	}
	else
	{
		m_renderer->SetUniform("BLINNPHONG_ENABLED", VALUEZERO);
	}
	m_renderer->SetUniform("LIGHT_POSITION", m_position);
	m_renderer->SetUniform("SPEC_POWER", m_specPower);
	m_renderer->SetUniform("SPEC_AMOUNT", m_specFactor);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Resets all values
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void PointLight::ResetValues()
{
	m_specFactor = 0;
	m_specPower  = 0;
	m_dot3Enabled = false;
	m_phongEnabled = false;
	m_BlinnphongEnabled = false;
	m_position = Vector3(0,0,0);
}

// CONSTRUCTOR
PointLight::PointLight(Renderer *renderer)
{
	m_renderer = renderer;
}

// DESTRUCTOR
PointLight::~PointLight()
{

}
