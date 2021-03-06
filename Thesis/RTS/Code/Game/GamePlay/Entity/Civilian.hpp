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

	TaskType GetTaskFromNNOutput(double &max);
	int		 GetGlobalBestScore();
	int		 GetLocalBestScore();
	void	 SetGlobalBestScore(int value);
	void	 SetLocalBestScore (int value);
	bool	 HasMoreResources();
	
	void EvaluateNN(Task * task,EntityState previousState,IntVector2 cords);
	void EvaluateMoveTask(EntityState prevState,IntVector2 location);
	void EvaluateIdleTask(EntityState prevState);
	void EvaluateGatherResourceFoodTask(EntityState prevState,IntVector2 cords);
	void EvaluateGatherResourceStoneTask(EntityState prevState, IntVector2 cords);
	void EvaluateGatherResourceWoodTask(EntityState prevState, IntVector2 cords);
	void EvaluateDropResourceTask(EntityState prevState,IntVector2 location);
	void EvaluateBuildHouseTask(EntityState prevState,IntVector2 location);
	void EvaluateBuildArmySpawnerTask(EntityState prevState,IntVector2 location);

	void TrainOnBuildHouseTask(EntityState previousState, IntVector2 cords);
	void TrainOnBuildArmySpawnerTask(EntityState previousState, IntVector2 cords);
	void TrainOnDropingResources(EntityState previousState, IntVector2 cords);
	void TrainToGatherAndDropResource(EntityState previousState, IntVector2 cords);

	void SetDesiredOutputToRandomTask();

	void SetDesiredOutputToMoveToTownCenter(float value,EntityState previousState,IntVector2 cords);
	void SetDesiredOuputForMoveToResourceTask  (float value, EntityState previousState, IntVector2 cords);

	void TrainNeuralNetwork(Task *task);
	
	void Render();

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