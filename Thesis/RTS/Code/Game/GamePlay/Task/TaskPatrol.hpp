#pragma once
#include "Game/GamePlay/Task/Task.hpp"
/*\class  : TaskPatrol	   
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 2/5/2019 4:41:56 PM
* \contact: srsrakhil@gmail.com
*/
enum PatrolSubType
{
	PATROL_MOVE_TO_START,
	PATROL_MOVE_TO_TARGET,
	PATROL_FINISH
};
class Map;
class Entity;
class TaskPatrol : public Task
{

public:
	//Member_Variables
	PatrolSubType m_subType;
	Vector2 m_patrolEndPosition;
	Vector2 m_patrolStartPosition;
	float   m_speed = 100;
	//Static_Member_Variables

	//Methods

	TaskPatrol(Map *map,Entity *entity);
	~TaskPatrol();

	void InitPatrolBehavior();
	void InitsStartPosition();
	void InitsEndPosition();
	
	bool DoTask(float delatTime);
	bool DoMoveTask(float deltaTime);

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