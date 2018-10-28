#include "Game/GamePlay/Task/TaskMove.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
// CONSTRUCTOR
TaskMove::TaskMove(Map* map,Entity *entity,Vector2 position)
{
	m_entity = entity;
	m_map    = map;
	SetTargetPosition(position);
	SetStartPosition(entity->GetPosition());
}

// DESTRUCTOR
TaskMove::~TaskMove()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Moves object from point A to B
*@param   : Delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskMove::DoTask(float deltaTime)
{
	Entity *targetPlaceEntity = m_map->GetEntityFromPosition(m_targetPosition);
	if(targetPlaceEntity != nullptr && targetPlaceEntity != m_entity)
	{
		m_entity->m_state.m_position = m_entity->GetPosition();
		return true;
	}
	Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction		= m_targetPosition - currentPosition;
	direction				= direction.GetNormalized();
	currentPosition			+= direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);

	Vector2 distance = m_entity->GetPosition() - m_targetPosition;
	if(distance.GetLength() < 1)
	{
		CheckAndUpdateResourcesUsed();
		m_entity->m_state.m_position = m_entity->GetPosition();
		return true;
	}
	return false;
}
