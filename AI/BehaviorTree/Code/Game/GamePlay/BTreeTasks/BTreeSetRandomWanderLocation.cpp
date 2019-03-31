#include "Game/GamePlay/BTreeTasks/BTreeSetRandomWanderLocation.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"
#include "Engine/Core/Blackboard.hpp"

#include "Game/GameCommon.hpp"
// CONSTRUCTOR
BTreeSetRandomWanderLocation::BTreeSetRandomWanderLocation()
{
	m_type = BTREE_NODE_ACTION;
	m_name = BTREE_SET_WANDER_LOC_NODE_NAME;
}

// DESTRUCTOR
BTreeSetRandomWanderLocation::~BTreeSetRandomWanderLocation()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS BTreeSetRandomWanderLocation::RunSubTree()
{
	Vector2 targetLocation = Vector2(GetRandomFloatInRange(0, 1000), GetRandomFloatInRange(0, 700));
	targetLocation = ClampVector2(targetLocation,g_gameMapExtends.mins,g_gameMapExtends.maxs);
	m_btree->m_blackBoard->SetValue(BTREE_WANDER_LOCATION, targetLocation.GetAsString());

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
BTREE_NODE_STATS BTreeSetRandomWanderLocation::ExecuteNode()
{
	return RunSubTree();
}
