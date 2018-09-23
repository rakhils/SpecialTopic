#include "Game/GamePlay/Task/TaskLongRangeAttack.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"

#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
TaskLongRangeAttack::TaskLongRangeAttack(Map* map, Entity *entity, int tileIndex)
{
	m_map = map;
	m_entity = entity;
	m_attackTile = tileIndex;

	if (m_map->IsNeighbours(m_map->GetCordinates(m_entity->GetPosition()), m_map->GetCordinates(tileIndex),2))
	{
		m_nearestAttackTile = m_map->GetTileIndex(m_entity->GetPosition());
		m_isValid = true;
		return;
	}

	IntVector2 tileCords = m_map->GetFreeNeighbourTile(m_map->GetMapPosition(m_attackTile),2);
	m_nearestAttackTile  = m_map->GetTileIndex(tileCords);

	if (m_nearestAttackTile != -1)
	{
		m_isValid = true;
	}
}

// DESTRUCTOR
TaskLongRangeAttack::~TaskLongRangeAttack()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskLongRangeAttack::DoTask(float deltaTime)
{
	UNUSED(deltaTime);
/*
	Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction = targetPosition - currentPosition;
	direction = direction.GetNormalized();
	currentPosition += direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);*/

	Vector2 targetPosition = m_map->GetMapPosition(m_nearestAttackTile);
	Vector2 distance = m_entity->GetPosition() - targetPosition;
	if (distance.GetLength() < 1)
	{
		m_entity->m_map->AttackOnPosition(m_attackTile, 1);
		m_entity->m_map->CreateExplosions(m_entity->m_map->GetMapPosition(m_attackTile));
		//return true;
	}
	return true;
}
