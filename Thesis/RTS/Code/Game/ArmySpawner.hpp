#pragma once
#include "Game/GamePlay/Entity/Entity.hpp"
/*\class  : ArmySpawner	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/29/2018 4:51:25 PM
* \contact: srsrakhil@gmail.com
*/

class ArmySpawner :public Entity
{

public:
	//Member_Variables
	//Static_Member_Variables

	//Methods

	ArmySpawner();
	ArmySpawner(Map *map, Vector2 position, int teamID);
	~ArmySpawner();

	void ProcessInput(float deltaTime);
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