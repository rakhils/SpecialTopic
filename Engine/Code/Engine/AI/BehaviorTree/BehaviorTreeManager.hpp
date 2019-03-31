#pragma once
#include <vector>
#include "Engine/AI/BehaviorTree/Node.hpp"
/*\class  : BehaviorTreeManager	   
* \group  : <GroupName>	   
* \brief  :	   
* \TODO:  :	   
* \note   :	   
* \author : Rakhil Soman	   
* \version: 1.0	   
* \date   : 3/24/2019 5:38:18 PM
* \contact: srsrakhil@gmail.com
*/
struct BehaviorTreeInstances
{
	BehaviorTree *	   m_btree;
};
class BehaviorTreeManager
{

public:
	//Member_Variables
	
	static std::vector<BehaviorTreeInstances> m_behaviorTreeInstances;
	//Static_Member_Variables

	//Methods

	BehaviorTreeManager();
	~BehaviorTreeManager();

	static void CreateBehaviorTreeInstance(BehaviorTree *btree);

	static void StopBehaviorTree(BehaviorTree *btree);
	static void RestartBehaviorTree(BehaviorTree *btree);

	static void Update(float deltaTime);
	static void Render();
	static void ExecuteActiveNodes(float deltaTime);
	static void ProcessActiveNodes(float deltaTime);
	static void CheckAndRestartBTree(float deltaTime);
	static void DeactivateAllSubtrees(Node *node);
	static void ExecuteSubTree(Node *node);
	static void ExecuteNextSubtree(Node* currentNodes);
	static void ClearTaskFromList(Node *node);
	
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