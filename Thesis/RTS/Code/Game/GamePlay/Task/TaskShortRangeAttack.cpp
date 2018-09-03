#include "Game/GamePlay/Task/TaskShortRangeAttack.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
// CONSTRUCTOR
TaskShortRangeAttack::TaskShortRangeAttack(Entity *entity, int tileIndex)
{
	m_entity = entity;
	m_attackTile = tileIndex;
}

// DESTRUCTOR
TaskShortRangeAttack::~TaskShortRangeAttack()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskShortRangeAttack::DoTask(float delatTime)
{
	m_entity->m_map->AttackOnPosition(m_attackTile, 1);
	m_entity->m_map->CreateExplosions(m_entity->m_map->GetMapPosition(m_attackTile));
	return true;
}
