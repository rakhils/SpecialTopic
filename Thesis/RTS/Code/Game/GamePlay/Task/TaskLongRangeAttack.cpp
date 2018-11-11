#include "Game/GamePlay/Task/TaskLongRangeAttack.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"

#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
TaskLongRangeAttack::TaskLongRangeAttack(Map* map, Entity *entity, int tileIndex)
{
	m_map = map;
	m_entity = entity;
	m_attackTile = tileIndex;
	m_targetPosition = m_map->GetMapPosition(m_attackTile);
	m_taskType = TASK_LONG_ATTACK;
	m_isValid = true;
}

// DESTRUCTOR
TaskLongRangeAttack::~TaskLongRangeAttack()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskLongRangeAttack::DoTask(float deltaTime)
{
	UNUSED(deltaTime);
	/*UNUSED(deltaTime);
	
	Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction       = targetPosition - currentPosition;
	direction				= direction.GetNormalized();
	currentPosition			+= direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);*/
	
	Entity *entityToBeAttacked = m_entity->m_map->GetEntityFromPosition(m_targetPosition);
	if (entityToBeAttacked != nullptr )
	{
		if(entityToBeAttacked->m_teamID != 0 && entityToBeAttacked->m_teamID != m_entity->m_teamID)
		{
			m_entity->m_map->CreateExplosions(m_targetPosition,Rgba::RED);
			m_entity->UpdateUnitStatForEnemiesAttacked(entityToBeAttacked,1);
			Entity *attackedEntity = m_entity->m_map->AttackOnPosition(m_targetPosition, 1);
			if (attackedEntity->m_health <= 0)
			{
				m_entity->UpdateUnitStatForEnemiesKilled(entityToBeAttacked,1);
			}
			CheckAndUpdateResourcesUsed();
		}
	}
	return true;
}
