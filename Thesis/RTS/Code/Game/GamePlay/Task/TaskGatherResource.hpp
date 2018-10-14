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
enum GatherResourceSubTask
{
	MOVE_TOWARDS_RESOURCE,
	GATHER_RESOURCE,
	MOVE_TOWARDS_STORAGE,
	DROP_RESOURCE
};
class Entity;
class TownCenter;
class TaskGatherResource : public Task
{

public:
	//Member_Variables
	GatherResourceSubTask	m_subTask;
	TownCenter*				m_townCenter			= nullptr;
	Entity *				m_resource				= nullptr;
	Vector2					m_resourcePosition;
	Vector2					m_storagePosition;
	float					m_speed					= 100;
	float					m_waitTimeAtResource    = 0.f;
	float					m_maxWaitTimeAtResource = 1;
	//Static_Member_Variables

	//Methods

	TaskGatherResource(Entity *entity);
	~TaskGatherResource();

	void SetResourcePosition(Vector2 position);
	void SetStoragePosition(Vector2 position);
	bool WaitAtResource(float deltaTime);
	void UpdateResourceStorageStat(Entity *entityResourceType,int count);
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