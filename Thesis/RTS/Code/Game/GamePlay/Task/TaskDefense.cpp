#include "Game/GamePlay/Task/TaskDefense.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Task/TaskMove.hpp"

TaskDefense::TaskDefense(Map *map, Entity *entity)
{
	m_map = map;
	m_entity = entity;
	InitFlockBehavior();
}

// DESTRUCTOR
TaskDefense::~TaskDefense()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskDefense::InitFlockBehavior()
{
	m_taskComplete = false;
	std::vector<Entity*> m_entityListAlliesNear = m_map->GetAllAlliesNearLocation(m_entity->m_teamID, m_entity->GetPosition(), 4);
	if (m_entityListAlliesNear.size() > 0)
	{
		int random = GetRandomIntLessThan(static_cast<int>(m_entityListAlliesNear.size()));
		IntVector2 freeCords = m_map->GetFreeNeighbourTile(m_entityListAlliesNear.at(random)->GetPosition(), 2);
		m_targetPosition = m_map->GetMapPosition(freeCords);
		return;
	}
	m_taskComplete = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskDefense::DoTask(float delatTime)
{
	return DoRetreat(delatTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskDefense::DoFlock(float deltaTime)
{
	if(m_taskComplete)
	{
		return true;
	}


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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskDefense::DoRetreat(float deltaTime)
{
	UNUSED(deltaTime);
	return true;
}
