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
	
	void EvaluateNN(Task * task,EntityState previousState,IntVector2 cords);
	void EvaluateMoveTask(EntityState prevState,IntVector2 location);
	void EvaluateIdleTask(EntityState prevState);
	void EvaluateGatherResourceTask(EntityState prevState,IntVector2 location);
	void EvaluateDropResourceTask(EntityState prevState,IntVector2 location);
	void EvaluateBuildHouseTask(EntityState prevState,IntVector2 location);
	void EvaluateBuildArmySpawnerTask(EntityState prevState,IntVector2 location);

	IntVector2 GetBestNeighbour();

	//void SetDesiredOutputToMoveToNeighbour();
	void SetDesiredOutputToMoveToSafeArea();
	void SetDesiredOutputToMoveToRandomArea();
	void SetDesiredOutputToRandomTask();

	void SetDesiredOuputForMoveToTownCenterTask(float value,EntityState previousState,IntVector2 cords);
	void SetDesiredOuputForMoveToResourceTask  (float value, EntityState previousState, IntVector2 cords);


	void EvaluateMoveTaskToTownCenter(EntityState prevState,IntVector2 cords);
	void EvaluateMoveTaskToResource(EntityState prevState,IntVector2 cords);

	void TrainNN(Task *task);
	
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