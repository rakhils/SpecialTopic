#include "Game/GamePlay/Utils/Explosion.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/Materials/Material.hpp"
// CONSTRUCTOR
Explosion::Explosion(Vector2 position)
{
	m_position = position;
}

// DESTRUCTOR
Explosion::~Explosion()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Explosion::Update(float deltaTime)
{
	if(m_currentRadius > g_radius)
	{
		m_direction = -1;
	}
	if(m_currentRadius < 0.f)
	{
		m_direction =  1;
		m_completed = true;
	}
	m_currentRadius += m_direction * 10 *deltaTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Explosion::Render()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Disc2 disc(m_position, m_currentRadius);
	g_theRenderer->DrawCircle(disc,Rgba::RED);
	delete defaultMaterial;
}
