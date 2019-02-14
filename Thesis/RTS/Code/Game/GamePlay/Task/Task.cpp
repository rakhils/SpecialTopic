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
	m_entity->m_state.m_resourceFoodUsed  += m_resourcesNeeded.m_food;
	m_entity->m_state.m_resourceStoneUsed += m_resourcesNeeded.m_stone;
	m_entity->m_state.m_resourceWoodUsed  += m_resourcesNeeded.m_wood;
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
/*DATE    : 2018/10/21
*@purpose : Check if enough resources is availble to perform task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Task::HasEnoughResources()
{
	if(((TownCenter*)m_entity->FindMyTownCenter())->m_resourceStat.m_food > m_resourcesNeeded.m_food)
	{
		if (((TownCenter*)m_entity->FindMyTownCenter())->m_resourceStat.m_stone > m_resourcesNeeded.m_stone)
		{
			if (((TownCenter*)m_entity->FindMyTownCenter())->m_resourceStat.m_wood > m_resourcesNeeded.m_wood)
			{
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/27
*@purpose : Check if a standalone entity is positioned in cords
*@param   : Cordinate
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Task::HasStandAloneEntity(IntVector2 cords)
{
	for(int index = 0;index < m_map->m_standAloneEntities.size();index++)
	{
		if (m_map->m_standAloneEntities.at(index)->GetCordinates() == cords)
		{
			return true;
		}
	}
	return false;
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
		return "TASK_MOVE            ";
		break;
	case TASK_GATHER_RESOURCE_FOOD:
		return "TASK_GATHER_FOOD      ";
		break;
	case TASK_GATHER_RESOURCE_STONE:
		return "TASK_GATHER_STONE     ";
		break;
	case TASK_GATHER_RESOURCE_WOOD:
		return "TASK_GATHER_WOOD      ";
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
	case TASK_ATTACK:
		return "TASK_ATTACK           ";
	case TASK_RETREAT:
		return "TASK_RETREAT          ";
	case TASK_FOLLOW:
		return "TASK_FOLLOW           ";
	case TASK_PATROL:
		return "TASK_PATROL           ";
	case TASK_EXPLORE:
		return "TASK_EXPLORE          ";
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
		return "M";
		break;
	case TASK_GATHER_RESOURCE_FOOD:
		return "G_F";
		break;
	case TASK_GATHER_RESOURCE_STONE:
		return "G_S";
		break;
	case TASK_GATHER_RESOURCE_WOOD:
		return "G_W";
		break;
	case TASK_DROP_RESOURCE:
		return "D";
		break;
	case TASK_BUILD_TOWNCENTER:
		return "B_TC";
		break;
	case TASK_BUILD_HOUSE:
		return "B_H";
		break;
	case TASK_BUILD_ARMY_SPAWNER:
		return "B_AS";
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
		return "I";
	case TASK_ATTACK:
		return "A";
	case TASK_RETREAT:
		return "R";
	case TASK_FOLLOW:
		return "F";
	case TASK_PATROL:
		return "P";
	case TASK_EXPLORE:
		return "E";
	default:
		break;
	}
	return "#";
}
