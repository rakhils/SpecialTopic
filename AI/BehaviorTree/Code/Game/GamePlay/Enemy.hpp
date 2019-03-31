#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"
#include "Engine/AI/BehaviorTree/BehaviorTreeManager.hpp"

#include "Game/GamePlay/BTreeTasks/BTreeMoveTask.hpp"
/*\class  : Enemy	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman	   
* \version: 1.0	   
* \date   : 3/24/2019 7:30:20 PM
* \contact: srsrakhil@gmail.com
*/
class Map;
enum ENEMY_STATE
{
	WANDER,
	ALERT,
	ATTACK,
	PURSUIT,
	RETREAT
};
class Enemy
{

public:
	//Member_Variables
	Vector2 m_position;
	float   m_speed;
	BehaviorTree *m_btree = nullptr;
	Map *m_map            = nullptr;
	float m_lastAttackTime = 0;
	//Static_Member_Variables

	//Methods

	Enemy();
	~Enemy();
	
	void InitBTree();
	void InitBlackboardValues();

	void Update(float deltaTime);
	void UpdateBlackboardValues();
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