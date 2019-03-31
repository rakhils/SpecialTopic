#pragma once
#include "Engine/AI/BehaviorTree/Decorators/BTreeDecorator.hpp"
/*\class  : BTreeBlackboardCondition   
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 3/28/2019 1:54:32 AM
* \contact: srsrakhil@gmail.com
*/
 
class BTreeBlackboardCondition : public BTreeDecorator
{

public:
	//Member_Variables
	int m_enemyStateEnumValue       = 0;
	int m_enemyStateTargetEnumValue = 0;
	//Static_Member_Variables

	//Methods

	BTreeBlackboardCondition();
	BTreeBlackboardCondition(std::string name,BehaviorTree *btree);
	~BTreeBlackboardCondition();

	bool CheckAndReturnResult();
	
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