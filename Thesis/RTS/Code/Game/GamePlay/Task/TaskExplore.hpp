#pragma once
#include "Game/GamePlay/Task/Task.hpp"
/*\class  : TaskExplore		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 2/5/2019 7:45:16 PM
* \contact: srsrakhil@gmail.com
*/
class Map;
class Entity;
class TaskExplore : public Task
{

public:
	//Member_Variables
	float m_speed = 100;
	//Static_Member_Variables

	//Methods

	TaskExplore(Map *map,Entity *entity);
	~TaskExplore();
	
	void InitExploreBehaviour();

	bool DoTask(float deltaTime);
	bool DoExploreMoveTask(float deltaTime);

	void CheckAndPushExploredPlace();
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