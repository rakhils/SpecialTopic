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
	if (!HasEnoughResources())
	{
		return true;
	}
	m_buildDelay += deltaTime;
	if (m_buildDelay < m_buildMaxDelay)
	{
		return false;
	}
	if (HasStandAloneEntity(m_entity->GetCordinates()))
	{
		return true;
	}
	m_buildDelay = 0;
	m_map->CreateArmySpawner(m_entity->GetPosition(), m_entity->m_teamID);
	m_entity->UpdateUnitStatForArmySpawnerBuilt(1);
	CheckAndUpdateResourcesUsed();
	return true;
}

