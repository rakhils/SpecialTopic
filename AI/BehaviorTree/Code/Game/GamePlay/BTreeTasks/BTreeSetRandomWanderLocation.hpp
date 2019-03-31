#pragma once
#include "Engine/AI/BehaviorTree/Node.hpp"
/*\class  : BTreeSetRandomWanderLocation	   
* \group  : <GroupName>		   
* \brief  :	   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0	   
* \date   : 3/27/2019 8:55:10 PM
* \contact: srsrakhil@gmail.com
*/
 
class BTreeSetRandomWanderLocation : public Node
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	BTreeSetRandomWanderLocation();
	~BTreeSetRandomWanderLocation();
	
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