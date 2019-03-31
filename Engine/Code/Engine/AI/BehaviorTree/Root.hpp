#pragma once
#include "Engine/AI/BehaviorTree/Node.hpp"
/*\class  : Root	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/24/2019 8:34:37 PM
* \contact: srsrakhil@gmail.com
*/
 
class Root : public Node
{

public:
	//Member_Variables
	Node * m_child;
	//Static_Member_Variables

	//Methods

	Root();
	~Root();
	
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