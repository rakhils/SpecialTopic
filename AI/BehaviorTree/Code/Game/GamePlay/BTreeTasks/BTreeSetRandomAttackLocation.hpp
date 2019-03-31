#pragma once
#include "Engine/AI/BehaviorTree/Node.hpp"
/*\class  : BTreeSetRandomAttackLocation	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/27/2019 9:10:15 PM
* \contact: srsrakhil@gmail.com
*/
class Player;
class BTreeSetRandomAttackLocation : public Node
{

public:
	//Member_Variables
	Player * m_player = nullptr;
	//Static_Member_Variables

	//Methods

	BTreeSetRandomAttackLocation();
	~BTreeSetRandomAttackLocation();

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