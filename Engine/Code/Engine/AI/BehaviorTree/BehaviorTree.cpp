#include "Engine/AI/BehaviorTree/BehaviorTree.hpp"
#include "Engine/AI/BehaviorTree/Node.hpp"
#include "Engine/AI/BehaviorTree/Root.hpp"
#include "Engine/AI/BehaviorTree/Composite.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
BehaviorTree::BehaviorTree()
{
	m_root = new Root();
	m_root->m_name = "ROOT";

	m_blackBoard = new Blackboard();
}

// DESTRUCTOR
BehaviorTree::~BehaviorTree()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Updates btree
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTree::Update(float deltaTime)
{
	UNUSED(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Renders BTree
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTree::Render(Vector2 startPosition)
{
	RenderTree(m_root,0,1,startPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTree::RenderTree(Node* node,int depth,int childCount,Vector2 position)
{
	RenderNode(node,depth,childCount,position,position);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 BehaviorTree::RenderCompositeNode(Node* node,int depth,int childCount,Vector2 position)
{
	float xOffSet = 0;
	float yOffset = 0;
	int horizontalOffset = 350;
	size_t totalChildCount = node->m_parent->m_children.size();

	if(totalChildCount == 1)
	{
		xOffSet = 0;
	}
	else
	{
		if (childCount <  totalChildCount / 2.f)
		{
			xOffSet = -1.f * static_cast<float>((static_cast<int>(GetFlooredValue(totalChildCount / 2.f) - childCount))) * static_cast<float>(horizontalOffset);
		}
		else
		{
			xOffSet = 1.f * static_cast<float>((static_cast<int>(childCount - GetFlooredValue(totalChildCount / 2.f))))  * static_cast<float>(horizontalOffset);
		}
	}

	if (depth == 2)
	{
		xOffSet = xOffSet * 1 / (depth);
	}
	if (depth == 3)
	{
		xOffSet = xOffSet * 1 / (depth - 1);
	}
	Rgba color = Rgba::WHITE;
	if (node->m_isActiveNode)
	{
		color = Rgba::YELLOW;
	}
	if (node->m_isActiveForFrame && node->m_frameTime > 0)
	{
		node->m_frameTime -= static_cast<float>(Clock::GetMasterClock()->frame.m_seconds);
		if (node->m_frameTime < 0)
		{
			node->m_isActiveForFrame = false;
		}
		color = Rgba::YELLOW;
	}
	
	position += Vector2(xOffSet, yOffset);
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawAABB(AABB2(position, m_boxWidth, m_boxHeight), color);
	delete defaultMaterial;

	Vector2 textPosition = position;
	
	if(node->m_type == BTREE_NODE_SELECTOR || node->m_type == BTREE_NODE_SEQUENCE || node->m_type == BTREE_NODE_SIMPLE_PARALLEL)
	{
		for(int decoratorIndex = 0;decoratorIndex < ((Composite*)node)->m_decorators.size();decoratorIndex++)
		{
			BTreeDecorator* decorator = ((Composite*)node)->m_decorators.at(decoratorIndex);

			Material *defaultMaterialDecorator = Material::AquireResource("default");
			Renderer::GetInstance()->BindMaterial(defaultMaterialDecorator);
			Renderer::GetInstance()->DrawAABB(AABB2(position + Vector2(0,25), m_boxWidth, m_boxHeight/2.f), Rgba::BLUE);
			delete defaultMaterialDecorator;

			Vector3 xOffSetVector(25, 0, 0);
			Material *textMaterialDecorator = Material::AquireResource("Data\\Materials\\text.mat");
			Renderer::GetInstance()->BindMaterial(textMaterialDecorator);
			Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(position) + Vector3(0,25,0) - xOffSetVector, Vector3::RIGHT, Vector3::UP, decorator->m_name, 5, Rgba::RED);
			delete textMaterialDecorator;
		}

		for (int serviceIndex = 0; serviceIndex < ((Composite*)node)->m_services.size(); serviceIndex++)
		{
			BTreeService *service = ((Composite*)node)->m_services.at(serviceIndex);

			Material *defaultMaterialService = Material::AquireResource("default");
			Renderer::GetInstance()->BindMaterial(defaultMaterialService);
			Renderer::GetInstance()->DrawAABB(AABB2(position + Vector2(0,25), m_boxWidth, m_boxHeight/2.f), Rgba::GREEN);
			delete defaultMaterialService;

			Vector3 xOffSetVector(25, 0, 0);
			Material *textMaterialService = Material::AquireResource("Data\\Materials\\text.mat");
			Renderer::GetInstance()->BindMaterial(textMaterialService);
			Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(position) + Vector2(0,25) - xOffSetVector, Vector3::RIGHT, Vector3::UP, service->m_name, 5, Rgba::RED);
			delete textMaterialService;
		}
	}
	Vector3 xOffSetVector(25, 0, 0);
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(textPosition) - xOffSetVector, Vector3::RIGHT, Vector3::UP, node->m_name, 6, Rgba::RED);
	delete textMaterial;



	return position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTree::RenderActionNode(Node* node,int depth,int childCount,Vector2 position)
{
	float xOffSet = 0;
	float yOffset = 0;
	int   horizontalOffset = 350;
	size_t totalChildCount = node->m_parent->m_children.size();
	
	if(childCount < totalChildCount/2.f)
	{
		xOffSet = -1 * (totalChildCount / 2.f - childCount) * horizontalOffset;
	}
	else
	{
		xOffSet = 1 * (childCount - totalChildCount / 2.f) *  horizontalOffset;
	}
	if(depth == 2)
	{
		xOffSet = xOffSet * 1 / (depth);
	}
	if (depth == 3)
	{
		xOffSet = xOffSet * 1 / (depth - 1);
	}

	Rgba color = Rgba::WHITE;
	if(node->m_isActiveNode)
	{
		color = Rgba::YELLOW;
	}
	if (node->m_isActiveForFrame && node->m_frameTime > 0)
	{
		node->m_frameTime -= static_cast<float>(Clock::GetMasterClock()->frame.m_seconds);
		if(node->m_frameTime < 0)
		{
			node->m_isActiveForFrame = false;
		}
		color = Rgba::YELLOW;
	}
	position += Vector2(xOffSet, yOffset);
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawAABB(AABB2(position, m_boxWidth, m_boxHeight), color);
	delete defaultMaterial;

	Vector3 xOffSetVector(25, 0, 0);
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(position) - xOffSetVector, Vector3::RIGHT, Vector3::UP, node->m_name, 6, Rgba::RED);
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTree::RenderRootNode(Node* node,int depth,int childCount, Vector2 position)
{
	UNUSED(childCount);
	UNUSED(depth);
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawAABB(AABB2(position, m_boxWidth, m_boxHeight), Rgba::WHITE);
	delete defaultMaterial;

	Vector3 xOffSet(25, 0, 0);
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(position) - xOffSet, Vector3::RIGHT, Vector3::UP, node->m_name, 6, Rgba::RED);
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorTree::RenderNode(Node* node, int depth, int childCount, Vector2 position,Vector2 parentPosition)
{
	if(node->m_type == BTREE_NODE_ACTION)
	{
		Vector2 drawPosition = RenderCompositeNode(node, depth, childCount,position + Vector2(0,0));
		Material *defaultMaterial = Material::AquireResource("default");
		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		Renderer::GetInstance()->DrawLine(drawPosition, parentPosition);
		delete defaultMaterial;
		return;
	}
	if(node->m_type == BTREE_NODE_SELECTOR || node->m_type == BTREE_NODE_SEQUENCE || node->m_type == BTREE_NODE_SIMPLE_PARALLEL)
	{
		
		Vector2 drawPosition = RenderCompositeNode(node, depth, childCount,position);
		Material *defaultMaterial = Material::AquireResource("default");
		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		Renderer::GetInstance()->DrawLine(drawPosition, parentPosition);
		delete defaultMaterial;



		Vector2 offset(0, -150);
		if (depth == 2 && childCount == 2)
		{
			offset.x += 175;
		}
		for(int index = 0;index < node->m_children.size();index++)
		{
			RenderNode(node->m_children.at(index), depth + 1, index,drawPosition + offset,drawPosition);
		}
	}
	if (node->m_type == BTREE_NODE_ROOT)
	{
		RenderRootNode(node, depth, childCount,position);
		RenderNode(node->m_children.at(0), depth,1,position + Vector2(0,-100),position);
	}
}
