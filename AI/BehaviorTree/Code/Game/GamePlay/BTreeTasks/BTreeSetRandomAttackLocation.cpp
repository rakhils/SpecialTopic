#include "Game/GamePlay/BTreeTasks/BTreeSetRandomAttackLocation.hpp"
#include "Game/GamePlay/Entity/Player.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"

// CONSTRUCTOR
BTreeSetRandomAttackLocation::BTreeSetRandomAttackLocation()
{
	m_type = BTREE_NODE_ACTION;
	m_name = BTREE_SET_ATTACK_LOC_NODE_NAME;
}

// DESTRUCTOR
BTreeSetRandomAttackLocation::~BTreeSetRandomAttackLocation()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS BTreeSetRandomAttackLocation::RunSubTree()
{
	Vector2 playerPosition = m_player->m_position;
	Vector2 targetLocation = playerPosition + 
							 Vector2(GetRandomFloatInRange(-g_attackRange, g_attackRange), GetRandomFloatInRange(-g_attackRange, g_attackRange));

	targetLocation = ClampVector2(targetLocation,g_gameMapExtends.mins, g_gameMapExtends.maxs);
	m_btree->m_blackBoard->SetValue(BTREE_ATTACK_LOCATION, targetLocation.GetAsString());

	m_isActiveForFrame = true;
	m_frameTime = 0.25;
	
	return BTREE_NODE_RESULT_FINISHED_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS BTreeSetRandomAttackLocation::ExecuteNode()
{
	return RunSubTree();
}
