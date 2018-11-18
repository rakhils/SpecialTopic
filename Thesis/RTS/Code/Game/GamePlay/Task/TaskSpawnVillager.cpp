#include "Game/GamePlay/Task//TaskSpawnVillager.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
TaskSpawnVillager::TaskSpawnVillager(Map* map,TownCenter *townCenter)
{
	m_entity	 = townCenter;
	m_map		 = map;
	m_resourcesNeeded.m_food = 10;
	m_resourcesNeeded.m_wood = 10;
	m_targetPosition = townCenter->GetPosition();
	m_taskType = TASK_SPAWN_VILLAGER;
}

// DESTRUCTOR
TaskSpawnVillager::~TaskSpawnVillager()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Do task spawn villager
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskSpawnVillager::DoTask(float deltaTime)
{
	UNUSED(deltaTime);
	IntVector2 position = m_map->GetFreeNeighbourTile(m_entity->GetPosition());
	if (position == IntVector2(-1, -1))
	{
		DebugDraw::GetInstance()->DebugRenderLogf(1, "CANNOT SPAWN VILLAGER NO SPACE NEARBY ");
		return true;
	}
	if(!CheckAndReduceResources())
	{
		return true;
	}
	int tileIndex = m_map->GetTileIndex(m_entity->GetPosition());
	IntVector2 tileCords = m_map->GetCordinates(tileIndex);
	m_entity->UpdateUnitStatForVillagerSpawned(1);
	m_map->CreateCivilian(m_map->GetMapPosition(position), m_entity->m_teamID);
	return true;
}
