#include "Game/GamePlay/BTreeTasks/Decorators/BTreeBlackboardCondition.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"
// CONSTRUCTOR
BTreeBlackboardCondition::BTreeBlackboardCondition()
{

}


BTreeBlackboardCondition::BTreeBlackboardCondition(std::string name,BehaviorTree *btree)
{
	m_name = name;
	m_btree = btree;
}

// DESTRUCTOR
BTreeBlackboardCondition::~BTreeBlackboardCondition()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BTreeBlackboardCondition::CheckAndReturnResult()
{
	m_enemyStateEnumValue = m_btree->m_blackBoard->GetValue("ENEMY_STATE",(int)-1);
	if (m_enemyStateTargetEnumValue == m_enemyStateEnumValue)
	{
		return true;
	}
	return false;
}
