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
	TASK_GATHER_RESOURCE_FOOD,
	TASK_GATHER_RESOURCE_STONE,
	TASK_GATHER_RESOURCE_WOOD,
	TASK_DROP_RESOURCE,
	TASK_BUILD_TOWNCENTER,
	TASK_BUILD_HOUSE,
	TASK_BUILD_ARMY_SPAWNER,
	TASK_LONG_ATTACK,
	TASK_SHORT_ATTACK,
	TASK_SPAWN_VILLAGER,
	TASK_SPAWN_CLASSA_WARRIOR,
	TASK_SPAWN_CLASSB_WARRIOR,
	TASK_IDLE,
	TASK_ATTACK,
	TASK_RETREAT,
	TASK_FOLLOW,
	TASK_PATROL,
	TASK_EXPLORE,
	TASK_MOVEX,
	TASK_MOVEY,
	TASK_INVALID
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
	Vector2   m_targetPosition;
	Vector2   m_startPosition;
	bool	  m_taskComplete;
	//Static_Member_Variables

	//Methods

	Task();
	~Task();

	bool CheckAndReduceResources();
	void CheckAndUpdateResourcesUsed();

	void SetTargetPosition(Vector2 position);
	void SetStartPosition(Vector2 position);

	bool HasEnoughResources();
	bool HasStandAloneEntity(IntVector2 cords);

	void Update(float deltaTime);
	virtual bool DoTask(float deltaTime) = 0;

	//Static_Methods
	static std::string GetTaskTypeAsString(TaskType type);
	static std::string GetTaskTypeAsShortString(TaskType type);

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