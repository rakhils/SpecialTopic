#include "Entity.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Net/NetSession.hpp"

#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GameCommon.hpp"

Entity::Entity() : Entity(g_playerStart)
{

}

Entity::Entity(Vector2 position)
{
	SetPosition(position);
	Init();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::Init()
{
	m_velocity = Vector2(GetRandomFloatInRange(-1, 1), GetRandomFloatInRange(-1, 1));
	m_velocity = m_velocity.GetNormalized();
	m_velocity = m_velocity *m_maxSpeed;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Process all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::ProcessInputs(float deltaTime)
{
	UNUSED(deltaTime);
}

void Entity::Update(float deltaTime)
{
	//m_angle = Atan2Degrees(m_velocity.y, m_velocity.x);
	//m_forward = Vector2(CosDegrees(m_angle), SinDegrees(m_angle));

	m_velocity += (m_acceleration);
	m_velocity.Limit(m_maxSpeed);

	m_position += m_velocity * deltaTime;

	m_acceleration = Vector2::ZERO;
	m_angle = Atan2Degrees(m_velocity.y, m_velocity.x);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::ApplyForce(Vector2 force)
{
	m_acceleration += force;
}

void Entity::Render()
{
	Vector2 leftDirection (CosDegrees(m_angle + 80), SinDegrees(m_angle + 80));
	Vector2 rightDirection(CosDegrees(m_angle - 80), SinDegrees(m_angle - 80));
	Vector2 upDriection   (CosDegrees(m_angle), SinDegrees(m_angle));

	Vector2 nose		  = m_position  + upDriection    * m_scale * 1.5;
	Vector2 leftPosition  = m_position  + leftDirection  * m_scale/2.f;
	Vector2 rightPosition = m_position  + rightDirection * m_scale/2.f;

	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	Renderer::GetInstance()->DrawSolidTriangle(nose, leftPosition, rightPosition, Rgba::YELLOW);

	delete defaultMaterial;
}


void Entity::SetMap(Map *map)
{
	m_map = map;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/05/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetPosition(Vector2 position)
{
	m_position = position;
}
