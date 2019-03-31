#include "Game/GamePlay/BTreeTasks/BTreeMoveTask.hpp"
#include "Game/GamePlay/Entity/Player.hpp"
#include "Game/GamePlay/Enemy.hpp"
#include "Game/GameCommon.hpp"

BTreeMoveTask::BTreeMoveTask(BehaviorTree *btree)
{
	m_type = BTREE_NODE_ACTION;
	m_name = BTREE_MOVE_NODE_NAME;
	m_btree = btree;
}

// DESTRUCTOR
BTreeMoveTask::~BTreeMoveTask()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS BTreeMoveTask::RunSubTree()
{
	Vector2 targetPosition  = m_btree->m_blackBoard->GetValue(m_blackBoardKey,Vector2::ZERO);
	Vector2 enemyPosition   = m_enemy->m_position;

	Vector2 distance =  targetPosition - enemyPosition;
	if(distance.GetLength() < g_unitDistance/2.f)
	{
		m_stats = BTREE_NODE_RESULT_FINISHED_SUCCESS;
	}
	else
	{
		m_stats = BTREE_NODE_RESULT_RUNNING;
	}

	distance = distance.GetNormalized();
	m_enemy->m_position += distance;
	return m_stats;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS BTreeMoveTask::ExecuteNode()
{
	return RunSubTree();
}
