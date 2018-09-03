#pragma once
#include "Game/GamePlay/Entity/Entity.hpp"

/*\class  : ClassBWarrior   
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 8/19/2018 12:12:21 AM
* \contact: srsrakhil@gmail.com
*/
class Map;
class ClassBWarrior : public Entity
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	ClassBWarrior();
	ClassBWarrior(Map *map,Vector2 position, int teamID);
	~ClassBWarrior();
	
	bool IsInRange(IntVector2 cords);
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