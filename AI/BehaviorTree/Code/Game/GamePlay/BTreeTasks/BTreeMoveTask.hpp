#pragma once
#include "Engine/AI/BehaviorTree/Node.hpp"
/*\class  : BTreeMoveTask		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/24/2019 8:22:57 PM
* \contact: srsrakhil@gmail.com
*/
class Player;
class Enemy;
class BTreeMoveTask : public Node
{

public:
	//Member_Variables
	Player * m_player = nullptr;
	Enemy  * m_enemy  = nullptr;
	std::string m_blackBoardKey = "";
	//Static_Member_Variables

	//Methods

	BTreeMoveTask(BehaviorTree *btree);
	~BTreeMoveTask();
	
	BTREE_NODE_STATS RunSubTree();
	BTREE_NODE_STATS ExecuteNode();
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