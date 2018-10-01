#include "Game/GamePlay/Task/TaskMove.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
// CONSTRUCTOR
TaskMove::TaskMove(Map* map,Entity *entity,Vector2 position)
{
	m_entity = entity;
	m_map    = map;
	SetTargetPosition(position);
}

// DESTRUCTOR
TaskMove::~TaskMove()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Sets the final position
*@param   : Target
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskMove::SetTargetPosition(Vector2 position)
{
	int tileIndex    = m_map->GetTileIndex(position);
	m_targetPosition = m_map->GetMapPosition(tileIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Moves object from point A to B
*@param   : Delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskMove::DoTask(float deltaTime)
{
	Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction		= m_targetPosition - currentPosition;
	direction				= direction.GetNormalized();
	currentPosition			+= direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);

	Vector2 distance = m_entity->GetPosition() - m_targetPosition;
	if(distance.GetLength() < 1)
	{
		CheckAndUpdateResourcesUsed();
		return true;
	}
	return false;
}
