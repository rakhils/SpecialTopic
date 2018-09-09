#include "Game/GamePlay/Task/TaskSpawnClassAWarrior.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/DebugDraw.hpp"
// CONSTRUCTOR
TaskSpawnClassAWarrior::TaskSpawnClassAWarrior(Map *map, Entity *entity)
{
	m_map = map;
	m_entity = entity;
}

// DESTRUCTOR
TaskSpawnClassAWarrior::~TaskSpawnClassAWarrior()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : Does the task
*@param   : NIL
*@return  : If task completed true else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskSpawnClassAWarrior::DoTask(float deltaTime)
{
	UNUSED(deltaTime);
	IntVector2 position = m_map->GetFreeNeighbourTile((m_entity->GetPosition()));
	if(position == IntVector2(-1,-1))
	{
		DebugDraw::GetInstance()->DebugRenderLogf(1, "CANNOT SPAWN VILLAGER NO SPACE NEARBY ");
		return true;
	}
	m_map->CreateClassAWarrior(m_map->GetMapPosition(position),m_entity->m_teamID);
	return true;
}
