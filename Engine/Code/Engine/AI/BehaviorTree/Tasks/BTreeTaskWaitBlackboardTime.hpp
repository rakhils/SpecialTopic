#pragma once
#include "Engine/AI/BehaviorTree/Node.hpp"
/*\class  : BTreeTaskWaitBlackboardTime	   
* \group  : <GroupName>	   
* \brief  :	   
* \TODO:  :   
* \note   :   
* \author : Rakhil Soman   
* \version: 1.0  
* \date   : 3/28/2019 9:40:43 PM
* \contact: srsrakhil@gmail.com
*/
class BTreeTaskWaitBlackboardTime : public Node
{

public:
	//Member_Variables
	std::string m_blackBoardKey;
	float       m_time = 0.f;
	//Static_Member_Variables

	//Methods

	BTreeTaskWaitBlackboardTime();
	BTreeTaskWaitBlackboardTime(BehaviorTree *btree);
	~BTreeTaskWaitBlackboardTime();

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