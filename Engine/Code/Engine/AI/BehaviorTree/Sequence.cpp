#include "Engine/AI/BehaviorTree/Sequence.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"
// CONSTRUCTOR
Sequence::Sequence()
{
	m_type = BTREE_NODE_SEQUENCE;
}

Sequence::Sequence(std::string name, BehaviorTree *btree)
{
	m_name = name;
	m_btree = btree;
	m_type = BTREE_NODE_SEQUENCE;
}

// DESTRUCTOR
Sequence::~Sequence()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Sequence::AddChild(Node *child)
{
	m_children.push_back(child);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Runs sequence node
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS Sequence::RunSubTree()
{
	if(!ExecuteDecorators())
	{
		return BTREE_NODE_RESULT_FINISHED_FAILURE;
	}
	UpdateServices();
	for (int childIndex = m_currentStartChildIndex; childIndex < m_children.size(); childIndex++)
	{
		Node *child = m_children.at(childIndex);
		BTREE_NODE_STATS stats = child->RunSubTree();
		if (stats == BTREE_NODE_RESULT_FINISHED_FAILURE)
		{
			SetStats(stats);
			return m_stats;
		}
		if (stats == BTREE_NODE_RESULT_RUNNING)
		{
			SetStats(stats);
			if (!child->m_isActiveNode && child->m_type == BTREE_NODE_ACTION)
			{
				m_btree->m_activeNodes.push_back(child);
				child->ActivateNode();
			}
			return m_stats;
		}
	}
	SetStats(BTREE_NODE_RESULT_FINISHED_SUCCESS);
	return m_stats;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS Sequence::ExecuteNode()
{
	if (!ExecuteDecorators())
	{
		return BTREE_NODE_RESULT_FINISHED_FAILURE;
	}
	UpdateServices();
	return BTREE_NODE_RESULT_FINISHED_SUCCESS;
}
