#include "Engine/AI/BehaviorTree/Node.hpp"

// CONSTRUCTOR
Node::Node()
{

}

// DESTRUCTOR
Node::~Node()
{

}

void Node::ActivateNode()
{
	m_isActiveNode = true;
}

void Node::DeactivateNode()
{
	m_isActiveNode = false;
}

void Node::SetStats(BTREE_NODE_STATS stats)
{
	m_stats = stats;
}

void Node::Reset()
{
	m_currentStartChildIndex = 0;
}
