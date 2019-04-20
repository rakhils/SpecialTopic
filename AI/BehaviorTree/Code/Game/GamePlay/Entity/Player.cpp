#include "Game/GamePlay/Entity/Player.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"
// CONSTRUCTOR
Player::Player()
{
	m_radius = g_unitDistance / 2.f;
	m_position = Vector2(800, 300);
}

// DESTRUCTOR
Player::~Player()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::Update(float deltaTime)
{
	if(InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_D))
	{
		m_position = (m_position - Vector2(deltaTime * -m_speed, 0));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_S))
	{
		m_position = (m_position - Vector2(0, deltaTime * m_speed));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_W))
	{
		m_position = (m_position - Vector2(0,deltaTime * -m_speed));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_A))
	{
		m_position = (m_position - Vector2(deltaTime * m_speed, 0));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::Render()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	g_theRenderer->DrawSolidCircle(m_position, g_unitDistance/2.f,Rgba::WHITE);
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::Attack()
{
	if(m_isDead)
	{
		return;
	}
	m_health--;
	if(m_health < 0)
	{
		m_isDead = true;
	}
}
