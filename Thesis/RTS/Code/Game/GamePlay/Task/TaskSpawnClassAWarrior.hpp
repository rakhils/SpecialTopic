#pragma once
#include "Game/GamePlay/Task/Task.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"

/*\class  : TaskSpawnClassAWarrior		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/29/2018 4:52:40 PM
* \contact: srsrakhil@gmail.com
*/
class Map;
class Entity;
class TaskSpawnClassAWarrior : public Task
{

public:
	//Member_Variables

	//Static_Member_Variables

	//Methods
	TaskSpawnClassAWarrior(Map *map,Entity *entity);
	~TaskSpawnClassAWarrior();

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