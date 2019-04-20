#include "Game/GamePlay/Task/TaskExplore.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
// CONSTRUCTOR
TaskExplore::TaskExplore(Map *map, Entity *entity)
{
	m_map = map;
	m_entity = entity;
	m_taskType = TASK_EXPLORE;
	InitExploreBehaviour();
}

// DESTRUCTOR
TaskExplore::~TaskExplore()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : Inits exploration behavior.
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskExplore::InitExploreBehaviour()
{
	std::vector<IntVector2> freecords = m_map->GetAllNeighbouringCoordinates(m_entity->GetCordinates(), 4);
	m_targetPosition = m_entity->GetPosition();
	int retryCount = 0;
	while(retryCount < 5)
	{
		retryCount++;
		IntVector2 freeCord = freecords.at(static_cast<int>(GetRandomIntLessThan(static_cast<int>(freecords.size()))));
		std::vector<Entity*> entityList = m_map->GetAllEnemiesNearLocation(m_entity->m_teamID, m_map->GetMapPosition(freeCord), 1);
		if(entityList.size() > 0)
		{
			m_targetPosition = m_map->GetMapPosition(freeCord);
		}
	}
	m_targetPosition = m_map->GetMapPosition(m_map->GetRandomNeighbour(m_entity->FindEnemyTownCenter()->GetCordinates(), 5));
	if(m_map->m_personality == DEFENSIVE && m_entity->m_teamID == 2)
	{
		m_targetPosition = m_map->GetMapPosition(m_map->GetRandomNeighbour(m_entity->FindMyTownCenter()->GetCordinates(), 5));
	}
	retryCount = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : Do explore task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskExplore::DoTask(float deltaTime)
{
	return DoExploreMoveTask(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : Does movement task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskExplore::DoExploreMoveTask(float deltaTime)
{
	Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction = m_targetPosition - currentPosition;
	direction = direction.GetNormalized();
	currentPosition += direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);

	if (m_map->m_mapMode == MAP_MODE_TRAINING_AGGRESSIVE || m_map->m_mapMode == MAP_MODE_TRAINING_DEFENSIVE)
	{
		m_entity->SetPosition(m_targetPosition);
	}
	Vector2 distance = m_entity->GetPosition() - m_targetPosition;

	if (distance.GetLength() < 1)
	{
		CheckAndUpdateResourcesUsed();
		m_entity->m_state.m_position = m_entity->GetPosition();
		//CheckAndPushExploredPlace();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskExplore::CheckAndPushExploredPlace()
{
	std::vector<IntVector2> neighbouringCords = m_map->GetAllNeighbouringCoordinates(m_entity->GetCordinates(), 4);
	for(int index = 0;index < neighbouringCords.size();index++)
	{
		Entity *entity = m_map->GetEntityFromPosition(neighbouringCords.at(index));
		if(entity->m_type == TOWN_CENTER && entity->m_teamID != m_entity->m_teamID)
		{
			
		}
	}
}
