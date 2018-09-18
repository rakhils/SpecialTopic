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
			townCenter->m_resourceStat.m_food = finalFoodValue;
			townCenter->m_resourceStat.m_stone = finalStoneValue;
			townCenter->m_resourceStat.m_wood = finalWoodValue;
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
