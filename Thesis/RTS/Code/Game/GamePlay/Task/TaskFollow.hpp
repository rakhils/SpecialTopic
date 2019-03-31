#pragma once
#include "Game/GamePlay/Task/Task.hpp"
/*\class  : TaskFollow
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 2/1/2019 2:21:06 AM
* \contact: srsrakhil@gmail.com
*/
class TaskFollow : public Task
{

public:
	//Member_Variables
	float   m_speed = 100;
	Vector3   m_travelledDistance = Vector3::ZERO;
	//Static_Member_Variables

	//Methods

	TaskFollow(Map *map, Entity *entity);
	~TaskFollow();

	void InitFollowBehavior();

	bool DoTask(float delatTime);
	bool DoFollow(float deltaTime);

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