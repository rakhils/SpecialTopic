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
enum AttackSubTask
{
	ATTACK_SUBTASK_MOVE,ATTACK_SUBTASK_ATTACK,ATTACK_SUBTASK_NONE
};
class TaskAttack : public Task
{

public:
	//Member_Variables
	AttackSubTask m_subType;
	float		  m_speed = 100;
	Entity *	  m_targetEntity = nullptr;
	int			  m_attackDistance = 0;
	//Static_Member_Variables

	//Methods

	TaskAttack(Map *map, Entity *entity,int attackDistance);
	~TaskAttack();

	void InitAttackTask();

	bool DoTask(float delatTime);
	bool DoMoveBehavior(float deltaTime);
	bool DoAttackBehavior(float deltaTime);

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