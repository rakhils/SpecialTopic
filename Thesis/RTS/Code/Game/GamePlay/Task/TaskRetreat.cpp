#include "Game/GamePlay/Task/TaskRetreat.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Task/TaskMove.hpp"

TaskRetreat::TaskRetreat(Map *map, Entity *entity)
{
	m_map = map;
	m_entity = entity;
	m_taskType = TASK_RETREAT;
	InitRetreatBehavior();
}

// DESTRUCTOR
TaskRetreat::~TaskRetreat()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskRetreat::InitRetreatBehavior()
{
	Vector2 position = m_entity->FindMyTownCenter()->GetPosition();
	std::vector<IntVector2> freeCords = m_map->GetAllNeighbouringCoordinates(m_map->GetCordinates(position), 2);
	IntVector2 freeCord;
	if(freeCords.size() > 0)
	{
		freeCord = freeCords.at(GetRandomIntLessThan(static_cast<int>(freeCords.size())));
	}
	m_targetPosition = m_map->GetMapPosition(freeCord);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskRetreat::DoTask(float deltaTime)
{
	Entity *targetPlaceEntity = m_map->GetEntityFromPosition(m_targetPosition);
	if (targetPlaceEntity != nullptr && targetPlaceEntity != m_entity)
	{
		m_entity->m_state.m_position = m_entity->GetPosition();
		return true;
	}
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
		m_taskComplete = true;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskRetreat::DoRetreat(float deltaTime)
{
	Entity *targetPlaceEntity = m_map->GetEntityFromPosition(m_targetPosition);
	if (targetPlaceEntity != nullptr && targetPlaceEntity != m_entity)
	{
		m_entity->m_state.m_position = m_entity->GetPosition();
		return true;
	}
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
		m_taskComplete = true;
		return true;
	}
	return false;
}
