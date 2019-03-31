#include "Game/GamePlay/BTreeTasks/Services/BTreeCheckAndSetEnemyState.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Enemy.hpp"

#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"
#include "Engine/Core/Blackboard.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"
#include "Engine/Core/StringUtils.hpp"
// CONSTRUCTOR
BTreeCheckAndSetEnemyState::BTreeCheckAndSetEnemyState()
{

}

// DESTRUCTOR
BTreeCheckAndSetEnemyState::~BTreeCheckAndSetEnemyState()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BTreeCheckAndSetEnemyState::Update()
{
	Vector2 playerPositon = m_btree->m_blackBoard->GetValue(BTREE_PLAYER_LOCATION,Vector2::ZERO);
	Vector2 enemyPosition = m_btree->m_blackBoard->GetValue(BTREE_ENEMY_LOCATION, Vector2::ZERO);

	float distance = (playerPositon - enemyPosition).GetLength();
	if(distance < g_attackRange)
	{
		m_btree->m_blackBoard->SetValue(BTREE_ENEMY_STATE, ToString((int)ATTACK));
		return;
	}
	if (distance < g_alertRange)
	{
		m_btree->m_blackBoard->SetValue(BTREE_ENEMY_STATE, ToString((int)ALERT));
		return;
	}
	ENEMY_STATE state = (ENEMY_STATE)m_btree->m_blackBoard->GetValue(BTREE_ENEMY_STATE, -1);
	if(state == ATTACK || state == ALERT)
	{
		m_btree->m_blackBoard->SetValue(BTREE_PURSUIT_LOCATION,playerPositon.ToString());
		m_btree->m_blackBoard->SetValue(BTREE_ENEMY_STATE, ToString((int)PURSUIT));
		return;
	}
	Vector2 spawnLocation = m_btree->m_blackBoard->GetValue(BTREE_SPAWN_LOCATION, Vector2::ZERO);
	if((spawnLocation - enemyPosition).GetLength() > g_retreatRange)
	{
		m_btree->m_blackBoard->SetValue(BTREE_ENEMY_STATE, ToString((int)RETREAT));
		return;
	}
	m_btree->m_blackBoard->SetValue(BTREE_ENEMY_STATE, ToString((int)WANDER));
}
