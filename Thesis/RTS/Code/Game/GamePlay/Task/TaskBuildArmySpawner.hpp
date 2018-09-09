#pragma once
#include "Game/GamePlay/Task/Task.hpp"
#include "Engine/Math/Vector2.hpp"
/*\class  : TaskBuildArmySpawner	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 9/8/2018 8:56:12 PM
* \contact: srsrakhil@gmail.com
*/
class Map;
class Entity;
class TaskBuildArmySpawner : public Task
{

public:
	//Member_Variables
	Vector2 m_buildPosition;
	float   m_buildDelay    = 0;
	float   m_buildMaxDelay = 1;
	float   m_speed			= 100;
	//Static_Member_Variables

	//Methods

	TaskBuildArmySpawner(Map *map, Entity *entity,Vector2 position);
	~TaskBuildArmySpawner();
	
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