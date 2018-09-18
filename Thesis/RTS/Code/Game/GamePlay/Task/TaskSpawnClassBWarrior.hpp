#pragma once
#include "Game/GamePlay/Task/Task.hpp"
/*\class  : TaskSpawnClassBWarrior	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/29/2018 4:52:55 PM
* \contact: srsrakhil@gmail.com
*/
class Map;
class Entity;
class TaskSpawnClassBWarrior : public Task
{

public:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	TaskSpawnClassBWarrior(Map *map,Entity *entity);
	~TaskSpawnClassBWarrior();

	bool DoTask(float delatTime);
	
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