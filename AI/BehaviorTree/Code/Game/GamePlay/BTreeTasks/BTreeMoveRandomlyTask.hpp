#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/AI/BehaviorTree/Node.hpp"
/*\class  : MoveRandomly	   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/27/2019 7:54:00 PM
* \contact: srsrakhil@gmail.com
*/
class Enemy;
enum MOVE_RANDOMLY_STAGE
{
	MOVE_RANDOMLY_CHOOSE_LOCATION,
	MOVE_RANDOMLY_MOVE
};
class BTreeMoveRandomlyTask : public Node
{

public:
	//Member_Variables
	Vector2 m_targetLocation;
	Enemy * m_enemy = nullptr;
	MOVE_RANDOMLY_STAGE m_stage;
	//Static_Member_Variables

	//Methods

	BTreeMoveRandomlyTask();
	~BTreeMoveRandomlyTask();
	
	BTREE_NODE_STATS Run();
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