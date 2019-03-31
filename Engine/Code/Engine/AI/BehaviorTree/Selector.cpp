#include "Engine/AI/BehaviorTree/Selector.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"

// CONSTRUCTOR
Selector::Selector()
{
	m_type = BTREE_NODE_SELECTOR;
}

Selector::Selector(std::string name, BehaviorTree *btree)
{
	m_name = name;
	m_btree = btree;
	m_type = BTREE_NODE_SELECTOR;
}

// DESTRUCTOR
Selector::~Selector()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Selector::AddChild(Node * node)
{
	m_children.push_back(node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Run a selector node
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS Selector::RunSubTree()
{
	if(!ExecuteDecorators())
	{
		return BTREE_NODE_RESULT_FINISHED_FAILURE;
	}
	UpdateServices();
	for(int childIndex = m_currentStartChildIndex;childIndex < m_children.size(); childIndex++)
	{
		Node *child = m_children.at(childIndex);
		BTREE_NODE_STATS stats = child->RunSubTree();
		if(stats == BTREE_NODE_RESULT_FINISHED_SUCCESS)
		{
			SetStats(stats);
			return m_stats;
		}
		if (stats == BTREE_NODE_RESULT_RUNNING )
		{
			SetStats(stats);
			if (!child->m_isActiveNode && child->m_type == BTREE_NODE_ACTION)
			{
				ActivateNode();
				m_btree->m_activeNodes.push_back(child);
			}
			return m_stats;
		}
	}
	SetStats(BTREE_NODE_RESULT_FINISHED_FAILURE);
	return m_stats;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS Selector::ExecuteNode()
{
	if (!ExecuteDecorators())
	{
		return BTREE_NODE_RESULT_FINISHED_FAILURE;
	}
	UpdateServices();
	return BTREE_NODE_RESULT_FINISHED_SUCCESS;
}

