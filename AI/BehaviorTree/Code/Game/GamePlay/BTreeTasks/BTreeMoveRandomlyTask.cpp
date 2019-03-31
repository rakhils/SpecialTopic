#include "Game/GamePlay/BTreeTasks/BTreeMoveRandomlyTask.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Enemy.hpp"
#include "Engine/Math/MathUtil.hpp"
// CONSTRUCTOR
BTreeMoveRandomlyTask::BTreeMoveRandomlyTask()
{
	m_type = BTREE_NODE_ACTION;
	m_name = "MOVE_RANDOMLY_TASK";
}

// DESTRUCTOR
BTreeMoveRandomlyTask::~BTreeMoveRandomlyTask()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BTREE_NODE_STATS BTreeMoveRandomlyTask::Run()
{
	switch(m_stage)
	{
	case MOVE_RANDOMLY_CHOOSE_LOCATION:
		{
			m_targetLocation = Vector2(GetRandomFloatInRange(0, 1000), GetRandomFloatInRange(0, 700));
			m_stage = MOVE_RANDOMLY_MOVE;
		}
		break;
	case MOVE_RANDOMLY_MOVE:
		{
			Vector2 playerPosition = m_enemy->m_position;
			Vector2 distance =  m_targetLocation - playerPosition;
			if (distance.GetLength() < g_unitDistance / 2.f)
			{
				m_stage = MOVE_RANDOMLY_CHOOSE_LOCATION;
			}
			m_enemy->m_position += distance.GetNormalized();
		}
		break;
	}
	return BTREE_NODE_RESULT_FINISHED_SUCCESS;
}
