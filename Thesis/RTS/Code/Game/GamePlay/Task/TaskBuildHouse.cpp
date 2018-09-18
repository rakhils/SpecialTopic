#include "Game/GamePlay/Task/TaskBuildHouse.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"

TaskBuildHouse::TaskBuildHouse(Map *map, Entity *entity, Vector2 position)
{
	m_map = map;
	m_entity = entity;
	m_buildPosition = position;
	m_resourcesNeeded.m_stone = 2;
	m_resourcesNeeded.m_wood  = 2;
}

// DESTRUCTOR
TaskBuildHouse::~TaskBuildHouse()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskBuildHouse::DoTask(float deltaTime)
{
	IntVector2 entityPosition = m_map->GetTilePosition(m_entity->GetPosition());
	IntVector2 buildPosition = m_map->GetTilePosition(m_buildPosition);
	if (m_map->IsNeighbours(buildPosition, entityPosition))
	{
		m_buildDelay += deltaTime;
		if (m_buildDelay < m_buildMaxDelay)
		{
			return false;
		}
		m_buildDelay = 0;
		m_map->CreateHouse(m_buildPosition, m_entity->m_teamID);
		IntVector2 freeNeighbour = m_map->GetFreeNeighbourTile(m_buildPosition);
		if (freeNeighbour == IntVector2(-1, -1))
		{
			return true;
		}
		m_entity->SetPosition(m_map->GetMapPosition(freeNeighbour));
		return true;
	}
	Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction = m_buildPosition - currentPosition;
	direction = direction.GetNormalized();
	currentPosition += direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);

	Vector2 distance = m_entity->GetPosition() - m_buildPosition;
	if (distance.GetLength() < 1)
	{
		return true;
	}
	return false;
}

