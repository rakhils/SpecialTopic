#include "Engine/AI/BehaviorTree/BehaviorTreeManager.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"
#include "Engine/Core/EngineCommon.hpp"
std::vector<BehaviorTreeInstances> BehaviorTreeManager::m_behaviorTreeInstances;
// CONSTRUCTOR
BehaviorTreeManager::BehaviorTreeManager()
{

}

// DESTRUCTOR
BehaviorTreeManager::~BehaviorTreeManager()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Creates new btree instance
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::CreateBehaviorTreeInstance(BehaviorTree *btree)
{
	BehaviorTreeInstances btreeInstance;
	btreeInstance.m_btree = btree;
	m_behaviorTreeInstances.push_back(btreeInstance);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Stops executing behavior tree
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::StopBehaviorTree(BehaviorTree *btree)
{
	for(int index = 0;index < m_behaviorTreeInstances.size();index++)
	{
		if(m_behaviorTreeInstances.at(index).m_btree == btree)
		{
			m_behaviorTreeInstances.erase(m_behaviorTreeInstances.begin() + index, m_behaviorTreeInstances.begin() + index + 1);
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : Restarts the BTree pushes root to active
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::RestartBehaviorTree(BehaviorTree *btree)
{
	btree->m_root->RunSubTree();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Updates all behavior trees
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::Update(float deltaTime)
{
	ExecuteActiveNodes(deltaTime);
	ProcessActiveNodes(deltaTime);
	CheckAndRestartBTree(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::Render()
{
	for (int btreeIndex = 0; btreeIndex < m_behaviorTreeInstances.size(); btreeIndex++)
	{
		m_behaviorTreeInstances.at(btreeIndex).m_btree->Render(Vector2::ZERO);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Process current active nodes
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::ExecuteActiveNodes(float deltaTime)
{
	UNUSED(deltaTime);
	for (int btreeIndex = 0; btreeIndex < m_behaviorTreeInstances.size(); btreeIndex++)
	{
		for (int activeNodeIndex = 0; activeNodeIndex < m_behaviorTreeInstances.at(btreeIndex).m_btree->m_activeNodes.size(); activeNodeIndex++)
		{
			Node *currentRunningNode = m_behaviorTreeInstances.at(btreeIndex).m_btree->m_activeNodes.at(activeNodeIndex);
			ExecuteSubTree(currentRunningNode);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Process active nodes
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::ProcessActiveNodes(float deltaTime)
{
	UNUSED(deltaTime);

	for (int btreeIndex = 0; btreeIndex < m_behaviorTreeInstances.size(); btreeIndex++)
	{
		for (int activeNodeIndex = 0; activeNodeIndex < m_behaviorTreeInstances.at(btreeIndex).m_btree->m_activeNodes.size(); activeNodeIndex++)
		{
			if (m_behaviorTreeInstances.at(btreeIndex).m_btree->m_activeNodes.at(activeNodeIndex)->m_stats != BTREE_NODE_RESULT_RUNNING)
			{
				Node *currentNode = m_behaviorTreeInstances.at(btreeIndex).m_btree->m_activeNodes.at(activeNodeIndex);
				currentNode->m_isActiveNode = false;
				DeactivateAllSubtrees(currentNode);
				ExecuteNextSubtree(currentNode);

				BehaviorTree *btree = m_behaviorTreeInstances.at(btreeIndex).m_btree;
				btree->m_activeNodes.erase(btree->m_activeNodes.begin() + activeNodeIndex,btree->m_activeNodes.begin() + activeNodeIndex + 1);
				activeNodeIndex--;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::CheckAndRestartBTree(float deltaTime)
{
	UNUSED(deltaTime);
	for (int btreeIndex = 0; btreeIndex < m_behaviorTreeInstances.size(); btreeIndex++)
	{
		if(m_behaviorTreeInstances.at(btreeIndex).m_btree->m_activeNodes.size() == 0)
		{
			RestartBehaviorTree(m_behaviorTreeInstances.at(btreeIndex).m_btree);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::DeactivateAllSubtrees(Node *node)
{
	node->m_isActiveNode = false;
	node->m_currentStartChildIndex = 0;
	for(int index = 0;index < node->m_children.size();index++)
	{
		DeactivateAllSubtrees(node->m_children.at(index));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::ExecuteSubTree(Node *node)
{
	if (node->m_type == BTREE_NODE_ROOT)
	{
		node->RunSubTree();
		return;
	}
	std::vector<Node*> treePaths;
	Node *tempNode = node;
	while(tempNode != nullptr)
	{
		treePaths.push_back(tempNode);
		tempNode = tempNode->m_parent;
	}
	
	for(int pathIndex = static_cast<int>(treePaths.size()) - 1;pathIndex >= 0;pathIndex--)
	{
		Node *currentNode = treePaths.at(pathIndex);
		BTREE_NODE_STATS stats = currentNode->ExecuteNode();
		if(stats == BTREE_NODE_RESULT_FINISHED_FAILURE)
		{
			for(int index = 0;index < treePaths.size();index++)
			{
				treePaths.at(index)->SetStats(BTREE_NODE_RESULT_FINISHED_FAILURE);
				treePaths.at(index)->DeactivateNode();
			}
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::ExecuteNextSubtree(Node * currentNode)
{
	if (currentNode->m_parent == nullptr)
	{
		return;
	}
	Node *parentNode = currentNode->m_parent;
	int childIndex = 0;
	for (childIndex = 0; childIndex < parentNode->m_children.size(); childIndex++)
	{
		if (parentNode->m_children.at(childIndex) == currentNode)
		{
			break;
		}
	}
	if (childIndex < parentNode->m_children.size() - 1)
	{
		parentNode->m_currentStartChildIndex = childIndex;
		parentNode->RunSubTree();
		return;
	}
	parentNode->m_currentStartChildIndex = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTreeManager::ClearTaskFromList(Node *node)
{
	for (int btreeIndex = 0; btreeIndex < m_behaviorTreeInstances.size(); btreeIndex++)
	{
		for (int activeNodeIndex = 0; activeNodeIndex < m_behaviorTreeInstances.at(btreeIndex).m_btree->m_activeNodes.size(); activeNodeIndex++)
		{
			if (m_behaviorTreeInstances.at(btreeIndex).m_btree->m_activeNodes.at(activeNodeIndex) == node)
			{
				BehaviorTree *btree = m_behaviorTreeInstances.at(btreeIndex).m_btree;
				btree->m_activeNodes.erase(btree->m_activeNodes.begin() + activeNodeIndex, btree->m_activeNodes.begin() + activeNodeIndex + 1);
				return;
			}
		}
	}
}
