#include "Game/GamePlay/Entity/Ship.hpp"

// CONSTRUCTOR
Ship::Ship() : GameObject("default")
{

}

// DESTRUCTOR
Ship::~Ship()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Ship::OnCollisionEnter(Collider* collider)
{
	int a = 1;
}
