#pragma once
#include "Engine/Math/Vector2.hpp"

#include "Game/GamePlay/Task/Task.hpp"
/*\class  : TaskGatherResource
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 8/21/2018 9:13:19 PM
* \contact: srsrakhil@gmail.com
*/
class Entity;
class TownCenter;
class TaskDropResource : public Task
{

public:
	//Member_Variables
	TownCenter*				m_townCenter = nullptr;
	float					m_speed = 100;
	//Static_Member_Variables

	//Methods

	TaskDropResource(Entity *entity, Entity* townCenter);
	~TaskDropResource();

	void UpdateResourceStorageStat(Entity *entityResourceType, int count);
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