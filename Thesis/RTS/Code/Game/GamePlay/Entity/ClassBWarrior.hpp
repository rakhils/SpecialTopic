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
	int m_enemiesAttacked			= 0;
	int m_enemiesKilled				= 0;
	int m_enemiesAttackedBeforeTask = 0;
	int m_enemiesKilledBeforeTask	= 0;
	//Static_Member_Variables

	//Methods

	ClassBWarrior();
	ClassBWarrior(Map *map,Vector2 position, int teamID);
	~ClassBWarrior();
	
	void ProcessInputs(float deltaTime);
	void Update(float deltaTime);
	void TrainNN();
	void Render();

	bool IsInRange(int tileIndex);
	bool IsInRange(Vector2 position);
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