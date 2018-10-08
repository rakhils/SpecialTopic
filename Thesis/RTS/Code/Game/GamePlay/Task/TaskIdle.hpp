#pragma once
#include "Game/GamePlay/Task/Task.hpp"
/*\class  : TaskIdle		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 9/30/2018 4:53:27 PM
* \contact: srsrakhil@gmail.com
*/
 
class TaskIdle : public Task
{

public:
	//Member_Variables
	float   m_delay = 0;
	float   m_maxDelay = -1;

	//Static_Member_Variables

	//Methods

	TaskIdle();
	~TaskIdle();
	
	//Static_Member_Variables

	//Methods

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