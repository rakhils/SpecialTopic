#pragma once
#include "Game/GamePlay/Task/Task.hpp"
/*\class  : TaskSpawnVillager
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 8/29/2018 4:52:16 PM
* \contact: srsrakhil@gmail.com
*/
class TownCenter;
class Map;
class TaskSpawnVillager : public Task
{

public:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	TaskSpawnVillager(Map *map,TownCenter *townCenter);
	~TaskSpawnVillager();

	bool DoTask(float deltaTime);
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