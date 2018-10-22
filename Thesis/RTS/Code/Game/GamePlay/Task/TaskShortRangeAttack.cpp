#include "Game/GamePlay/Task/TaskShortRangeAttack.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"

#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
TaskShortRangeAttack::TaskShortRangeAttack(Map* map,Entity *entity, int tileIndex)
{
	m_map        = map;
	m_entity     = entity;
	m_attackTile = tileIndex;
	m_targetPosition = m_map->GetMapPosition(m_attackTile);
	m_taskType = TASK_SHORT_ATTACK;
	m_isValid = true;
}

// DESTRUCTOR
TaskShortRangeAttack::~TaskShortRangeAttack()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskShortRangeAttack::DoTask(float deltaTime)
{
	UNUSED(deltaTime);
	/*UNUSED(deltaTime);
	
	Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction       = targetPosition - currentPosition;
	direction				= direction.GetNormalized();
	currentPosition			+= direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);*/

	Entity *attackedEntity = m_entity->m_map->AttackOnPosition(m_targetPosition, 1);
	if (attackedEntity != nullptr)
	{
		if(attackedEntity->m_teamID != 0 && attackedEntity->m_teamID != m_entity->m_teamID)
		{
			m_entity->m_map->CreateExplosions(m_entity->m_map->GetMapPosition(m_attackTile));
			m_entity->UpdateUnitStatForEnemiesAttacked(1);
			if (attackedEntity->m_health <= 0)
			{
				m_entity->UpdateUnitStatForEnemiesKilled(1);
			}
			CheckAndUpdateResourcesUsed();
		}
	}
	return true;
}
