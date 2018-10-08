#pragma once
#include "Game/GamePlay/Entity/Entity.hpp"
/*\class  : Civilian	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/18/2018 11:21:22 PM
* \contact: srsrakhil@gmail.com
*/
enum PendingTask
{
	NOTASK,
	BUILD_HOUSE,
	BUILD_ARMYCENTER
};
class Map;
class Civilian : public Entity
{

public:
	//Member_Variables
	PendingTask m_pendingTask;
	//Static_Member_Variables

	//Methods

	Civilian();
	Civilian(Map *map,Vector2 position,int teamID);
	~Civilian();

	void ProcessInputs(float deltaTime);
	void Update(float deltaTime);
	
	float EvaluateNN(Task * task,EntityState previousState);
	float EvaluateMoveTask(EntityState prevState);
	float EvaluateOnRandomMoveTask(EntityState prevState);
	float EvaluateIdleTask(EntityState prevState);
	void TrainNN(Task *task,float evaluationValue);
	
	void Render();

	void GatherResource(Vector2 resourcePosition);
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