#pragma once
#include "Game/GamePlay/Entity/Entity.hpp"
/*\class  : ClassAWarrior
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 8/19/2018 12:12:08 AM
* \contact: srsrakhil@gmail.com
*/
class Map;
class ClassAWarrior : public Entity
{

public:
	//Member_Variables
	float m_lastAttackTime = 0.f;
	//Static_Member_Variables

	//Methods

	ClassAWarrior();
	ClassAWarrior(Map *map,Vector2 position, int teamIndex);
	~ClassAWarrior();

	void	 ProcessInputs(float deltaTime);
	void	 Update(float deltaTime);
	TaskType GetTaskFromNNOutput(double &max);

	void	 InitDefenseNNModel();
	void     InitAttackModel();

	void	 TrainNN(Task *task);
	void	 EvaluateNN(Task *task, EntityState previousState, IntVector2 cords);
	void	 EvaluateMoveTask(EntityState previousState, IntVector2 cords);
	void	 EvaluateShortAttackTask(EntityState previousState, IntVector2 cords);
	void	 EvaluateIdleTask(EntityState previousState, IntVector2 cords);
			 
	void	 Render();
			 
	bool	 IsInRange(int tileIndex);
	bool	 IsInRange(Vector2 position);
	bool	 IsInRange(IntVector2 cords);

	
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