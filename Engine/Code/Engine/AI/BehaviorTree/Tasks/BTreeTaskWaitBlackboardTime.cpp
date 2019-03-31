#include "Engine/AI/BehaviorTree/Tasks/BTreeTaskWaitBlackboardTime.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"
#include "Engine/Time/Clock.hpp"
// CONSTRUCTOR
BTreeTaskWaitBlackboardTime::BTreeTaskWaitBlackboardTime()
{
	m_type = BTREE_NODE_ACTION;
	m_name = "BB_WAIT";
}

BTreeTaskWaitBlackboardTime::BTreeTaskWaitBlackboardTime(BehaviorTree *btree)
{
	m_btree = btree;
	m_time = m_btree->m_blackBoard->GetValue(m_blackBoardKey, 0.f);
	m_type = BTREE_NODE_ACTION;
	m_name = "BB_WAIT";
}

// DESTRUCTOR
BTreeTaskWaitBlackboardTime::~BTreeTaskWaitBlackboardTime()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS BTreeTaskWaitBlackboardTime::RunSubTree()
{
	if(m_time > 0.f)
	{
		m_time -= static_cast<float>(Clock::GetMasterClock()->frame.m_seconds);
		return BTREE_NODE_RESULT_RUNNING;
	}
	m_time = m_btree->m_blackBoard->GetValue(m_blackBoardKey, 0.f);
	return BTREE_NODE_RESULT_FINISHED_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS BTreeTaskWaitBlackboardTime::ExecuteNode()
{
	return RunSubTree();
}
