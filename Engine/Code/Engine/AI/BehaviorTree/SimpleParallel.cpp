#include "Engine/AI/BehaviorTree/SImpleParallel.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"
// CONSTRUCTOR
SimpleParallel::SimpleParallel()
{
	m_type = BTREE_NODE_SIMPLE_PARALLEL;
}

SimpleParallel::SimpleParallel(std::string name, BehaviorTree *btree)
{
	m_name = name;
	m_btree = btree;
	m_type = BTREE_NODE_SIMPLE_PARALLEL;
}

// DESTRUCTOR
SimpleParallel::~SimpleParallel()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Adds main child to be executed
*@param   : Main child
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SimpleParallel::AddChild(Node *child)
{
	m_children.push_back(child);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Runs simple parallel node
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS SimpleParallel::RunSubTree()
{
	if (!ExecuteDecorators())
	{
		return BTREE_NODE_RESULT_FINISHED_FAILURE;
	}
	UpdateServices();

	Node * mainTask = m_children.at(0);
	Node * backgroundTask = m_children.at(1);

	BTREE_NODE_STATS mainChildResult = mainTask->RunSubTree();

	if (mainChildResult == BTREE_NODE_RESULT_RUNNING)
	{
		if (!mainTask->m_isActiveNode && mainTask->m_type == BTREE_NODE_ACTION)
		{
			mainTask->ActivateNode();
			mainTask->SetStats(BTREE_NODE_RESULT_RUNNING);
			if (!m_isActiveNode)
			{
				m_btree->m_activeNodes.push_back(this);
				ActivateNode();
			}
		}
		backgroundTask->RunSubTree();
		if (!backgroundTask->m_isActiveNode)
		{
			backgroundTask->ActivateNode();
			backgroundTask->SetStats(BTREE_NODE_RESULT_RUNNING);
		}

		SetStats(BTREE_NODE_RESULT_RUNNING);
		return m_stats;
	}

	mainTask->DeactivateNode();
	/********************************************************************/
	switch (m_finishMode)
	{
	case SimpleParallel::DELAYED:
	{
		if (!backgroundTask->m_isActiveNode)
		{
			backgroundTask->ActivateNode();
			backgroundTask->SetStats(BTREE_NODE_RESULT_RUNNING);
			m_btree->m_activeNodes.push_back(backgroundTask);
		}
		SetStats(mainChildResult);
		return m_stats;
	}
	break;
	case SimpleParallel::IMMEDIATE:
		backgroundTask->SetStats(m_stats);
		DeactivateNode();
		SetStats(mainChildResult);
		return m_stats;
		break;
	default:
		break;
	}
	m_stats = mainChildResult;
	return mainChildResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS SimpleParallel::ExecuteNode()
{
	return RunSubTree();
}
