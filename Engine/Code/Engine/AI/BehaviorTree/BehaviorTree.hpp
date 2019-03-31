#pragma once
#include "Engine/AI/BehaviorTree/Node.hpp"
#include "Engine/AI/BehaviorTree/Root.hpp"
#include "Engine/Core/Blackboard.hpp"
#include "Engine/Math/Vector2.hpp"
/*\class  : BehaviorTree	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/24/2019 2:46:18 PM
* \contact: srsrakhil@gmail.com
*/
 
class BehaviorTree
{

public:
	//Member_Variables
	Root *				m_root		 = nullptr;
	Blackboard *		m_blackBoard = nullptr;

	Vector2			    m_rootPosition;
	std::vector<Node*>  m_activeNodes;

	// DRAW PART
	float   m_boxWidth  = 40;
	float   m_boxHeight = 25;
	int     m_subtreeCount[10];
	//Static_Member_Variables

	//Methods

	BehaviorTree();
	~BehaviorTree();
	
	void Update(float deltaTime);

	void CalculateDepth(Node *node,int depth,int childCount);

	void Render(Vector2 startPosition);
	void RenderTree			(Node* node,int depth,int childCount,Vector2 position);
	Vector2 RenderCompositeNode(Node* node,int depth,int childCount,Vector2 position);
	void RenderActionNode	(Node* node,int depth,int childCount,Vector2 position);
	void RenderRootNode		(Node* node,int depth,int childCount,Vector2 position);
	void RenderNode			(Node* node,int depth,int childCount,Vector2 position,Vector2 parentPosition);
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