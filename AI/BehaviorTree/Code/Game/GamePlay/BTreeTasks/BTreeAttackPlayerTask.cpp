#include "Game/GamePlay/BTreeTasks/BTreeAttackPlayerTask.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Enemy.hpp"
#include "Game/GamePlay/Entity/Player.hpp"
#include "Game/GameCommon.hpp"
// CONSTRUCTOR
BTreeAttackPlayerTask::BTreeAttackPlayerTask()
{
	m_type = BTREE_NODE_ACTION;
	m_name = BTREE_ATTACK_NODE_NAME;
}

// DESTRUCTOR
BTreeAttackPlayerTask::~BTreeAttackPlayerTask()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS BTreeAttackPlayerTask::RunSubTree()
{
	if(m_enemy->m_lastAttackTime <= 0)
	{
		m_enemy->m_lastAttackTime = g_attackInterval;
		Vector2 direction = m_player->m_position - m_enemy->m_position;
		direction = direction.GetNormalized();
		m_map->CreateBullet(m_enemy->m_position, direction);
	}
	m_stats = BTREE_NODE_RESULT_FINISHED_SUCCESS;
	m_isActiveForFrame = true;
	m_frameTime = 0.25;
	return m_stats;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS BTreeAttackPlayerTask::ExecuteNode()
{
	return RunSubTree();
}
