#pragma once
#include "Engine/AI/BehaviorTree/Composite.hpp"

/*\class  : SImpleParallel	   
* \group  : <GroupName>	   
* \brief  :	   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/24/2019 2:23:56 PM
* \contact: srsrakhil@gmail.com
*/

class SimpleParallel : public Composite
{
	enum FINISH_MODE
	{
		IMMEDIATE,
		DELAYED
	};
public:
	//Member_Variables
	FINISH_MODE m_finishMode;
	//Static_Member_Variables

	//Methods

	SimpleParallel();
	SimpleParallel(std::string name,BehaviorTree *btree);
	~SimpleParallel();

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