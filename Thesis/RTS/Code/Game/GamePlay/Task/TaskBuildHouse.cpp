#include "Game/GamePlay/Task/TaskBuildHouse.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"

TaskBuildHouse::TaskBuildHouse(Map *map, Entity *entity, Vector2 position)
{
	m_map = map;
	m_entity = entity;
	m_targetPosition = position;
	m_resourcesNeeded.m_stone = 2;
	m_resourcesNeeded.m_wood  = 2;
	m_taskType = TASK_BUILD_HOUSE;
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
	if(!HasEnoughResources())
	{
		return true;
	}
	m_buildDelay += deltaTime;
	if (m_buildDelay < m_buildMaxDelay)
	{
		return false;
	}
	m_buildDelay = 0;
	m_map->CreateHouse(m_entity->GetPosition(), m_entity->m_teamID);
	m_entity->UpdateUnitStatForHouseBuilt(1);
	CheckAndUpdateResourcesUsed();
	return true;
}

