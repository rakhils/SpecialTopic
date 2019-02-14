#pragma once
#include "Game/GamePlay/Task/Task.hpp"
/*\class  : TaskShortRangeAttack	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 9/1/2018 8:26:19 PM
* \contact: srsrakhil@gmail.com
*/
class TaskShortRangeAttack : public Task
{

public:
	//Member_Variables
	int m_attackTile = -1;
	float   m_speed = 100;
	int m_nearestAttackTile = -1;
	float m_time = 0.f;
	float m_totalTime = 1.f;
	//Static_Member_Variables

	//Methods

	TaskShortRangeAttack(Map* map,Entity *entity,int tileIndex);
	~TaskShortRangeAttack();

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