#include "Game/GamePlay/Task/TaskAttack.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Task/TaskMove.hpp"

TaskAttack::TaskAttack(Map *map, Entity *entity,int attackDistance)
{
	m_map = map;
	m_entity = entity;
	m_attackDistance = attackDistance;
	m_taskType = TASK_ATTACK;
	InitAttackTask();
}

// DESTRUCTOR
TaskAttack::~TaskAttack()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskAttack::InitAttackTask()
{
	m_targetEntity = nullptr;
	m_startPosition = m_entity->GetPosition();
	std::vector<Entity*> m_entityListEnemiesNear = m_map->GetAllEnemiesNearLocation(m_entity->m_teamID, m_entity->GetPosition(), 1);
	if(m_entityListEnemiesNear.size() != 0)
	{
		m_targetEntity = m_entityListEnemiesNear.at(0);
		return;
	}

	std::vector<Entity*> entityListEnemiesNearNew = m_map->GetAllEnemiesNearLocation(m_entity->m_teamID, m_entity->GetPosition(), 4);
	if (entityListEnemiesNearNew.size() > 0)
	{
		m_targetEntity = entityListEnemiesNearNew.at(GetRandomIntLessThan(static_cast<int>(entityListEnemiesNearNew.size())));
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskAttack::DoTask(float delatTime)
{
	if (m_targetEntity == nullptr || m_targetEntity->m_health <= 0)
	{
		return true;
	}
	return DoAttackBehavior(delatTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskAttack::DoMoveBehavior(float deltaTime)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskAttack::DoAttackBehavior(float deltaTime)
{
	IntVector2 startCords   = m_map->GetCordinates(m_startPosition);
	IntVector2 currentCords = m_entity->GetCordinates();
	IntVector2 targetCords  = m_targetEntity->GetCordinates();

	if(m_map->GetCellDistance(currentCords,startCords) > 2)
	{
		//return true;
	}
	if (m_map->GetCellDistance(currentCords, targetCords) > 4)
	{
		///return true;
	}

	// ATTACK
	if(m_map->GetCellDistance(targetCords,currentCords) <= m_attackDistance)
	{
		m_targetPosition = m_targetEntity->GetPosition();
		float currentTime = static_cast<float>(GetCurrentTimeSeconds());
		if(currentTime - m_entity->m_lastAttackTime  > 1)
		{
			Entity *attackedEntity = m_entity->m_map->AttackOnPosition(m_targetPosition, 1,m_entity->m_teamID);
			if(attackedEntity != nullptr && attackedEntity == m_targetEntity)
			{
				m_entity->m_map->CreateExplosions(m_targetPosition, m_entity->GetTeamColor());
				m_entity->UpdateUnitStatForEnemiesAttacked(attackedEntity, 1);
				m_entity->m_lastAttackTime = currentTime;
				if (attackedEntity->m_health <= 0)
				{
					m_entity->UpdateUnitStatForEnemiesKilled(attackedEntity, 1);
					m_targetEntity = nullptr;
				}
				CheckAndUpdateResourcesUsed();
				return true;
			}
			else
			{
				return true;
			}
		}
		return false;
	}

	// MOVE
	Vector2 currentPosition =  m_entity->GetPosition();
	Vector2 direction		=  m_targetEntity->GetPosition() - currentPosition;
	direction				=  direction.GetNormalized();
	currentPosition			+= direction * m_speed * deltaTime;
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
		return false;
	}
	return false;
}
