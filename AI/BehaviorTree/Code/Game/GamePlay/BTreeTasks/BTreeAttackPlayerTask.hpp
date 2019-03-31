#pragma once
#include "Engine/AI/BehaviorTree/Node.hpp"
/*\class  : BTreeAttackPlayerTask	   
* \group  : <GroupName
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 3/27/2019 9:31:53 PM
* \contact: srsrakhil@gmail.com
*/
class Player;
class Enemy;
class Map;
class BTreeAttackPlayerTask : public Node
{

public:
	//Member_Variables
	Player * m_player;
	Enemy  * m_enemy;
	Map    * m_map = nullptr;
	//Static_Member_Variables

	//Methods

	BTreeAttackPlayerTask();
	~BTreeAttackPlayerTask();

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