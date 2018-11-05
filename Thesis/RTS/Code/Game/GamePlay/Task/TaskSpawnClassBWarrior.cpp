#include "Game/GamePlay/Task/TaskSpawnClassBWarrior.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/DebugDraw.hpp"
// CONSTRUCTOR
TaskSpawnClassBWarrior::TaskSpawnClassBWarrior(Map *map, Entity *entity)
{
	m_map    = map;
	m_entity = entity;
	m_resourcesNeeded.m_food = 16;
	m_resourcesNeeded.m_wood = 16;
	m_targetPosition = entity->GetPosition();
}

// DESTRUCTOR
TaskSpawnClassBWarrior::~TaskSpawnClassBWarrior()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : Does the task
*@param   : NIL
*@return  : If task completed true else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskSpawnClassBWarrior::DoTask(float deltaTime)
{
	UNUSED(deltaTime);
	IntVector2 position = m_map->GetFreeNeighbourTile(m_entity->GetPosition());
	if (position == IntVector2(-1, -1))
	{
		DebugDraw::GetInstance()->DebugRenderLogf(1, "CANNOT SPAWN VILLAGER NO SPACE NEARBY ");
		return false;
	}
	m_entity->UpdateUnitStatForLongRangeArmySpawned(1);
	m_map->CreateClassBWarrior(m_map->GetMapPosition(position), m_entity->m_teamID);
	CheckAndUpdateResourcesUsed();
	return true;
}