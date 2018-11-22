#include "Game/GamePlay/Task/TaskBuildArmySpawner.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"

TaskBuildArmySpawner::TaskBuildArmySpawner(Map *map, Entity *entity,Vector2 position)
{
	m_map = map;
	m_entity = entity;
	m_targetPosition = position;
	m_resourcesNeeded.m_stone = 8;
	m_resourcesNeeded.m_wood  = 8;
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
	UNUSED(deltaTime);
	if (!HasEnoughResources())
	{
		return true;
	}
	if (HasStandAloneEntity(m_entity->GetCordinates()))
	{
		return true;
	}
	std::vector<Entity*> neighbourEntities = m_map->GetAllEntitiesNearLocation(m_entity->GetPosition(), 1);
	for (int index = 0; index < neighbourEntities.size(); index++)
	{
		if (neighbourEntities.at(index)->IsStationaryEntity())
		{
			return true;
		}
	}
	if (!CheckAndReduceResources())
	{
		return true;
	}
	m_map->CreateArmySpawner(m_entity->GetPosition(), m_entity->m_teamID);
	m_entity->UpdateUnitStatForArmySpawnerBuilt(1);
	CheckAndUpdateResourcesUsed();
	return true;
}

