#pragma once
#include "Engine/AI/BehaviorTree/Composite.hpp"
#include "Engine/AI/BehaviorTree/Decorators/BTreeDecorator.hpp"
/*\class  : Sequence	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/24/2019 2:23:35 PM
* \contact: srsrakhil@gmail.com
*/
 
class Sequence : public Composite
{

public:
	//Member_Variables
	//Static_Member_Variables

	//Methods

	Sequence();
	~Sequence();
	Sequence(std::string name, BehaviorTree *btree);

	void AddChild(Node *child);
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