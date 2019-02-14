#pragma once
#include "Game/GamePlay/Task/Task.hpp"
/*\class  : TaskDefense	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 2/1/2019 2:21:06 AM
* \contact: srsrakhil@gmail.com
*/

class TaskDefense : public Task
{

public:
	//Member_Variables
	float   m_speed = 100;
	//Static_Member_Variables

	//Methods

	TaskDefense(Map *map, Entity *entity);
	~TaskDefense();

	void InitFlockBehavior();
	void InitRetreatBehavior();

	bool DoTask(float delatTime);
	bool DoFlock(float deltaTime);
	bool DoRetreat(float deltaTime);
	
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