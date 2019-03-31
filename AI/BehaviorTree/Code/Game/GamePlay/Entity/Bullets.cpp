#include "Game/GamePlay/Entity/Bullets.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
// CONSTRUCTOR
Bullets::Bullets()
{
	m_startTime = 5.f;
}

// DESTRUCTOR
Bullets::~Bullets()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Bullets::Update(float deltaTime)
{
	m_position += m_direction * deltaTime * m_speed;
	if(m_startTime < 0)
	{
		m_isDead = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Bullets::Render()
{
	if(m_isDead)
	{
		return;
	}

	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	g_theRenderer->DrawCircle(m_position.x,m_position.y, m_radius,Rgba::RED,50);
	delete defaultMaterial;
}
