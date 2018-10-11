#include "Game/GamePlay/Task/Task.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Entity/TownCenter.hpp"
#include "Game/GamePlay/Maps/Map.hpp"

#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
Task::Task()
{

}

// DESTRUCTOR
Task::~Task()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/14
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Task::CheckAndReduceResources()
{
	int teamID = m_entity->m_teamID;
	TownCenter *townCenter = m_map->m_townCenters.at(teamID - 1);
	if(townCenter->m_health > 0)
	{
		int finalFoodValue  = townCenter->m_resourceStat.m_food  - m_resourcesNeeded.m_food;
		int finalStoneValue = townCenter->m_resourceStat.m_stone - m_resourcesNeeded.m_stone;
		int finalWoodValue  = townCenter->m_resourceStat.m_wood  - m_resourcesNeeded.m_wood;
		if(finalFoodValue >= 0 && finalStoneValue >= 0 && finalWoodValue >= 0)
		{
			townCenter->m_resourceStat.m_food  = finalFoodValue;
			townCenter->m_resourceStat.m_stone = finalStoneValue;
			townCenter->m_resourceStat.m_wood  = finalWoodValue;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/29
*@purpose : Updates the count for the resources used for this task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Task::CheckAndUpdateResourcesUsed()
{
	//m_entity->m_state.m_resourceFoodUsed  += m_resourcesNeeded.m_food;
	//m_entity->m_state.m_resourceStoneUsed += m_resourcesNeeded.m_stone;
	//m_entity->m_state.m_resourceWoodUsed  += m_resourcesNeeded.m_wood;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Task::SetTargetPosition(Vector2 position)
{
	m_targetPosition = position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Task::SetStartPosition(Vector2 position)
{
	m_startPosition = position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Task::Update(float deltaTime)
{
	UNUSED(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : Gets task type as string
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Task::GetTaskTypeAsString(TaskType type)
{
	switch (type)
	{
	case TASK_MOVE:
		return "TASK_MOVE             ";
		break;
	case TASK_GATHER_RESOURCE:
		return "TASK_GATHER           ";
		break;
	case TASK_DROP_RESOURCE:
		return "TASK_DROP             ";
		break;
	case TASK_BUILD_TOWNCENTER:
		return "TASK_BUILD_TOWNCENTER ";
		break;
	case TASK_BUILD_HOUSE:
		return "TASK_BUILD_HOUSE      ";
		break;
	case TASK_BUILD_ARMY_SPAWNER:
		return "TASK_BUILD_ARMY_CENTER";
		break;
	case TASK_LONG_ATTACK:
		return "TASK_LONG_ATTACK      ";
		break;
	case TASK_SHORT_ATTACK:
		return "TASK_SHORT_ATTACK     ";
		break;
	case TASK_SPAWN_VILLAGER:
		return "TASK_SPAWN_VILLAGER   ";
		break;
	case TASK_SPAWN_CLASSA_WARRIOR:
		return "TASK_SPAWN_CLASSA     ";
		break;
	case TASK_SPAWN_CLASSB_WARRIOR:
		return "TASK_SPAWN_CLASSB     ";
		break;
	case TASK_IDLE:
		return "TASK_IDLE             ";
	case TASK_MOVEX:
		return "TASK_MOVE X           ";
	case TASK_MOVEY:
		return "TASK_MOVE Y           ";
	default:
		break;
	}
	return     "INVALID               ";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Task::GetTaskTypeAsShortString(TaskType type)
{
	switch (type)
	{
	case TASK_MOVE:
		return "MV";
		break;
	case TASK_GATHER_RESOURCE:
		return "GATHER";
		break;
	case TASK_DROP_RESOURCE:
		return "DROP";
		break;
	case TASK_BUILD_TOWNCENTER:
		return "B_TC";
		break;
	case TASK_BUILD_HOUSE:
		return "B_H";
		break;
	case TASK_BUILD_ARMY_SPAWNER:
		return "B_A_C";
		break;
	case TASK_LONG_ATTACK:
		return "L_A";
		break;
	case TASK_SHORT_ATTACK:
		return "S_A";
		break;
	case TASK_SPAWN_VILLAGER:
		return "S_V";
		break;
	case TASK_SPAWN_CLASSA_WARRIOR:
		return "S_SA";
		break;
	case TASK_SPAWN_CLASSB_WARRIOR:
		return "S_LA";
		break;
	case TASK_IDLE:
		return "IDLE";
	default:
		break;
	}
	return "INVALID";
}
