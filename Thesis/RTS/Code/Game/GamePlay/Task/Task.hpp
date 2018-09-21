#pragma once
#include "Game/GameCommon.hpp"
/*\class  : Task	   
* \group  : <GroupName>		   
* \brief  :		   
 \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/19/2018 12:19:26 AM
* \contact: srsrakhil@gmail.com
*/
class Entity;
class Map;
enum TaskType
{
	TASK_MOVE,
	TASK_GATHER_RESOURCEA,
	TASK_GATHER_RESOURCEB,
	TASK_GATHER_RESOURCEC,
	TASK_BUILD_TOWNCENTER,
	TASK_BUILD_HOUSE,
	TASK_BUILD_ARMY_SPAWNER,
	TASK_ATTACK,
	TASK_SPAWN_VILLAGER,
	TASK_SPAWN_CLASSA_WARRIOR,
	TASK_SPAWN_CLASSB_WARRIOR,
};

class Task
{

public:
	//Member_Variables
	Entity * m_entity = nullptr;
	Map    * m_map    = nullptr;
	bool     m_isValid = false;
	TaskType m_taskType;
	Resources m_resourcesNeeded;
	//Static_Member_Variables

	//Methods

	Task();
	~Task();

	bool CheckAndReduceResources();

	void Update(float deltaTime);
	virtual bool DoTask(float delatTime) = 0;

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