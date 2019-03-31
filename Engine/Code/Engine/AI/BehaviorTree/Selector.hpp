#pragma once
#include "Engine/AI/BehaviorTree/Composite.hpp"
/*\class  : Selector	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/24/2019 2:22:49 PM
* \contact: srsrakhil@gmail.com
*/
 
class Selector : public Composite
{

public:
	//Member_Variables
	// 
	//Static_Member_Variables

	//Methods

	Selector();
	Selector(std::string name,BehaviorTree *btree);
	~Selector();

	void AddChild(Node * node);

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