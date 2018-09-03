#pragma once
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Engine/Math/Vector2.hpp"
/*\class  : House		   
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 8/21/2018 3:04:07 PM
* \contact: srsrakhil@gmail.com
*/
class Map;
class House : public Entity
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	House();
	House(Map* map,Vector2 position, int teamID);
	~House();

	void Update(float deltaTime);
	void Render();
	
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