#pragma once
#include "Game/GamePlay/Entity/Entity.hpp"
/*\class  : ArmySpawner	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/29/2018 4:51:25 PM
* \contact: srsrakhil@gmail.com
*/

class ArmySpawner :public Entity
{

public:
	//Member_Variables
	
	//Static_Member_Variables
	//Methods

	ArmySpawner();
	ArmySpawner(Map *map, Vector2 position, int teamID);
	~ArmySpawner();

	void ProcessInput(float deltaTime);
	void EvaluateNN(Task *task, EntityState previousState, IntVector2 cords);
	void EvaluateShortRangeArmySpawnTask(EntityState previousState, IntVector2 cords);
	void EvaluateLongRangeArmySpawnTask(EntityState previousState, IntVector2 cords);
	void EvaluateIdleTask(EntityState previousState, IntVector2 cords);

	int		 GetGlobalBestScore();
	int		 GetLocalBestScore();
	void		 SetGlobalBestScore(int value);
	void		 SetLocalBestScore (int value);
	TaskType GetTaskFromNNOutput(double &max);
	void Update(float deltaTime);
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