#include "Game/GamePlay/Task/TaskBuildArmySpawner.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"

TaskBuildArmySpawner::TaskBuildArmySpawner(Map *map, Entity *entity,Vector2 position)
{
	m_map = map;
	m_entity = entity;
	m_targetPosition = position;
	m_resourcesNeeded.m_stone = 5;
	m_resourcesNeeded.m_wood  = 5;
	m_taskType = TASK_BUILD_ARMY_SPAWNER;
}

// DESTRUCTOR
TaskBuildArmySpawner::~TaskBuildArmySpawner()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskBuildArmySpawner::DoTask(float deltaTime)
{
	if (true)
	{
		return true;
	}
	IntVector2 entityPosition = m_map->GetTilePosition(m_entity->GetPosition());
	IntVector2 buildPosition = m_map->GetTilePosition(m_targetPosition);
	if (m_map->IsNeighbours(buildPosition, entityPosition))
	{
		m_buildDelay += deltaTime;
		if (m_buildDelay < m_buildMaxDelay)
		{
			return false;
		}
		m_buildDelay = 0;
		m_map->CreateArmySpawner(m_targetPosition, m_entity->m_teamID);
		m_entity->UpdateUnitStatForArmySpawnerBuilt(1);
		CheckAndUpdateResourcesUsed();
		IntVector2 freeNeighbour = m_map->GetFreeNeighbourTile(m_targetPosition);
		if (freeNeighbour == IntVector2(-1, -1))
		{
			return true;
		}
		m_entity->SetPosition(m_map->GetMapPosition(freeNeighbour));
		return true;
	}
	/*Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction = m_buildPosition - currentPosition;
	direction = direction.GetNormalized();
	currentPosition += direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);

	Vector2 distance = m_entity->GetPosition() - m_buildPosition;
	if (distance.GetLength() < 1)
	{
		return true;
	}*/
	return true;
}

