#pragma once
#include <vector>
/*\class  : Node	   
* \group  : <GroupName>	   
* \brief  :	   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/24/2019 2:24:36 PM
* \contact: srsrakhil@gmail.com
*/
enum BTREE_NODE_STATS
{
	BTREE_NODE_RESULT_INVALID,
	BTREE_NODE_RESULT_RUNNING,
	BTREE_NODE_RESULT_FINISHED_SUCCESS,
	BTREE_NODE_RESULT_FINISHED_FAILURE
};
enum BTREE_NODE_TYPE
{
	BTREE_NODE_ROOT,
	BTREE_NODE_ACTION,
	BTREE_NODE_SELECTOR,
	BTREE_NODE_SEQUENCE,
	BTREE_NODE_SIMPLE_PARALLEL,
};
class BehaviorTree;
class Node
{

public:
	//Member_Variables
	BTREE_NODE_TYPE    m_type;
	BTREE_NODE_STATS   m_stats;
	std::string		   m_name;
	BehaviorTree *	   m_btree  = nullptr;
	Node *			   m_parent = nullptr;
	bool			   m_isActiveNode = false;
	int				   m_currentStartChildIndex = 0;

	bool			   m_isActiveForFrame = false;
	float			   m_frameTime		  = 0.0f;

	std::vector<Node*> m_children;

	//Static_Member_Variables

	//Methods

	Node();
	~Node();

	void ActivateNode();
	void DeactivateNode();
	void SetStats(BTREE_NODE_STATS stats);
	void Reset();

	virtual BTREE_NODE_STATS RunSubTree() = 0;
	virtual BTREE_NODE_STATS ExecuteNode() = 0;
	
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};