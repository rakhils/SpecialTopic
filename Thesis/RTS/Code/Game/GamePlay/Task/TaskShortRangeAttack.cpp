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
	if ((m_entity->m_lastAttackTime + 1) > static_cast<float>(GetCurrentTimeSeconds()))
	{
		return true;
	}
	Entity *entityToBeAttacked = m_entity->m_map->GetEntityFromPosition(m_targetPosition);

	if (entityToBeAttacked != nullptr )
	{
		if(entityToBeAttacked->m_teamID != 0 && entityToBeAttacked->m_teamID != m_entity->m_teamID)
		{
			m_entity->m_map->CreateExplosions(m_targetPosition,m_entity->GetTeamColor());
			m_entity->UpdateUnitStatForEnemiesAttacked(entityToBeAttacked,1);
			Entity *attackedEntity = m_entity->m_map->AttackOnPosition(m_targetPosition, 1);
			m_entity->m_lastAttackTime = static_cast<float>(GetCurrentTimeSeconds());
			if (attackedEntity->m_health <= 0)
			{
				m_entity->UpdateUnitStatForEnemiesKilled(entityToBeAttacked,1);
			}
			CheckAndUpdateResourcesUsed();
		}
	}
	return true;
}
