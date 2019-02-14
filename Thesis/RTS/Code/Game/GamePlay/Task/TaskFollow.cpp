#include "Game/GamePlay/Task/TaskFollow.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"

TaskFollow::TaskFollow(Map *map, Entity *entity)
{
	m_map = map;
	m_entity = entity;
	m_taskType = TASK_FOLLOW;
	InitFollowBehavior();
	m_entity->m_strategy = FOLLOW;
}

// DESTRUCTOR
TaskFollow::~TaskFollow()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskFollow::InitFollowBehavior()
{
	std::vector<Entity*> m_entityListAlliesNear = m_map->GetAllAlliesNearLocation(m_entity->m_teamID, m_entity->GetPosition(), 4);
	if (m_entityListAlliesNear.size() > 0)
	{
		int random = GetRandomIntLessThan(static_cast<int>(m_entityListAlliesNear.size()));
		std::vector<IntVector2> freeCords = m_map->GetAllNeighbouringCoordinates(m_entityListAlliesNear.at(random)->GetCordinates(), 1);
		IntVector2 freeCord = freeCords.at(GetRandomIntLessThan(static_cast<int>(freeCords.size())));
		m_targetPosition = m_map->GetMapPosition(freeCord);
		return;
	}
	m_targetPosition = m_entity->GetPosition();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskFollow::DoTask(float deltaTime)
{
	return DoFollow(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskFollow::DoFollow(float deltaTime)
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

	if (m_map->m_mapMode == MAP_MODE_TRAINING_RANDOM_MAP_GEN)
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