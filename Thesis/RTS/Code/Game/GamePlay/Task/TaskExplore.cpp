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
	m_entity->m_strategy = EXPLORE;
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
		if(entityList.size() == 0)
		{
			m_targetPosition = m_map->GetMapPosition(freeCord);
		}
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

	if (m_map->m_mapMode == MAP_MODE_TRAINING_RANDOM_MAP_GEN)
	{
		m_entity->SetPosition(m_targetPosition);
	}
	Vector2 distance = m_entity->GetPosition() - m_targetPosition;

	if (distance.GetLength() < 1)
	{
		CheckAndUpdateResourcesUsed();
		m_entity->m_state.m_position = m_entity->GetPosition();
		return true;
	}
	return false;
}
