#pragma once
#include "Engine/Math/Vector2.hpp"

#include "Game/GamePlay/Task/Task.hpp"
/*\class  : TaskMove	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/21/2018 4:12:43 PM
* \contact: srsrakhil@gmail.com
*/
 
class TaskMove : public Task
{

public:
	//Member_Variables
	float   m_speed = 100;
	//Static_Member_Variables

	//Methods

	TaskMove(Map* map,Entity *entity,Vector2 position);
	~TaskMove();

	void SetTargetPosition(Vector2 position);
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