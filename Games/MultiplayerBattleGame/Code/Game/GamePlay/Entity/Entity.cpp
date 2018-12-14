#include "Entity.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Net/NetSession.hpp"

#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GameCommon.hpp"

Entity::Entity(Vector2 position,EntityType type)
{
	m_disc2D.center = position;
	m_entityType	= type;
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
	m_netObjectSnapshot   = new EntitySnapShot_t();
	m_localEntitySnapshot = new EntitySnapShot_t();
	switch (m_entityType)
	{
	case PLAYER:
		m_width  = g_playerWidth;
		m_height = g_playerHeight;
		break;
	case BULLET:
		m_width  = g_bulletWidth;
		m_height = g_bulletHeight;
		break;
	case EXPLOSION:

		break;
	default:
		break;
	}
	
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
	if(m_health <= 0)
	{
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : Updates local player snapshot
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateLocalEntitySnapshot()
{
	m_localEntitySnapshot->m_position		= m_position;
	m_localEntitySnapshot->m_angle			= m_angle;
	m_localEntitySnapshot->m_health			= m_health;
}

void Entity::Render()
{
	if (m_health <= 0)
	{
		return;
	}	
}

void Entity::SetNetObject(NetObject *netObject)
{
	m_netObject = netObject;
}

void Entity::SetPosition(Vector2 position)
{
	m_position = position;
}

void Entity::SetAngle(float angle)
{
	m_angle = angle;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Sets team
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::ApplySnapshot(EntitySnapShot_t *playerSnapshot)
{
	if(playerSnapshot == nullptr)
	{
		return;
	}
	m_position	    = playerSnapshot->m_position;
	m_angle			= playerSnapshot->m_angle;
	m_health		= playerSnapshot->m_health;
}

void Entity::SetTeam(int team)
{
	m_teamID = team;
}

void Entity::SetHealth(float health)
{
	m_health = health;
}

void Entity::SetHeight(float height)
{
	m_height = height;
}

void Entity::SetWidth(float width)
{
	m_width = width;
}

void Entity::SetMap(Map *map)
{
	m_map = map;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::IsCurrentPlayer()
{
	if ((NetSession::GetInstance()->m_hostConnection->m_index == m_index))
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/12
*@purpose : Reduces health by damage amount
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::TakeDamage(float healthReduction)
{
	m_health -= healthReduction;
	if(m_health <= 0)
	{

	}
}
