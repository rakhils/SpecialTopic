#include "Game/GamePlay/Task/TaskPatrol.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"

// CONSTRUCTOR
TaskPatrol::TaskPatrol(Map *map,Entity *entity)
{
	m_map = map;
	m_entity = entity;
	InitPatrolBehavior();
	m_taskType = TASK_PATROL;
	m_subType = PATROL_MOVE_TO_TARGET;
	m_entity->m_strategy = PATROL;
}

// DESTRUCTOR
TaskPatrol::~TaskPatrol()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : Inits patrol behavior
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskPatrol::InitPatrolBehavior()
{
	InitsStartPosition();
	InitsEndPosition();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskPatrol::InitsStartPosition()
{
	IntVector2 townCenterCords = m_entity->FindMyTownCenter()->GetCordinates();
	std::vector<IntVector2> freeCords = m_map->GetAllNeighbouringCoordinates(townCenterCords, 2);
	if(freeCords.size() == 0)
	{
		m_patrolStartPosition = m_entity->GetPosition();
		return;
	}
	m_patrolStartPosition = m_map->GetMapPosition(freeCords.at(GetRandomIntLessThan(static_cast<int>(freeCords.size()))));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : Inits patrol end point in map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskPatrol::InitsEndPosition()
{
	IntVector2 townCenterCords = m_entity->FindMyTownCenter()->GetCordinates();
	std::vector<IntVector2> freeCords = m_map->GetAllNeighbouringCoordinates(townCenterCords, 5);
	if (freeCords.size() == 0)
	{
		m_patrolEndPosition = m_entity->GetPosition();
		return;
	}
	m_patrolEndPosition = m_map->GetMapPosition(freeCords.at(GetRandomIntLessThan(static_cast<int>(freeCords.size()))));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskPatrol::DoTask(float delatTime)
{
	switch (m_subType)
	{
	case PATROL_MOVE_TO_START:
		m_targetPosition = m_patrolStartPosition;
		return DoMoveTask(delatTime);
		break;
	case PATROL_MOVE_TO_TARGET:
		m_targetPosition = m_patrolEndPosition;
		return DoMoveTask(delatTime);
		break;
	case  PATROL_FINISH:
		return true;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskPatrol::DoMoveTask(float deltaTime)
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
		if(m_subType == PATROL_MOVE_TO_START)
		{
			return true;
			//m_subType = PATROL_MOVE_TO_TARGET;
			//InitPatrolBehavior();
		}
		else
		if(m_subType == PATROL_MOVE_TO_TARGET)
		{
			m_subType = PATROL_MOVE_TO_START;
		}
		return false;
	}
	return false;
}
