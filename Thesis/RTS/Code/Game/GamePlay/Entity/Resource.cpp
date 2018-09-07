#include "Game/GamePlay/Entity/Resource.hpp"

#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Task/TaskGatherResource.hpp"
// CONSTRUCTOR
Resource::Resource()
{

}

Resource::Resource(Map *map,Vector2 position, EntityType type)
{
	m_map  = map;
	m_type = type;
	SetPosition(position);
}

// DESTRUCTOR
Resource::~Resource()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Processes all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resource::ProcessInputs(float deltaTime)
{
	UNUSED(deltaTime);
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;

	/*if (InputSystem::GetInstance()->WasLButtonJustPressed())
	{
		if(g_currentSelectedEntity != nullptr && g_currentSelectedEntity->m_type == CIVILIAN)
		{
			g_currentSelectedEntity->EmptyTaskQueue();
			int tileIndex = m_map->GetTileIndex(mousePosition);
			Vector2 mapPosition = m_map->GetMapPosition(tileIndex);
			Task *task = new TaskGatherResource(g_currentSelectedEntity, mapPosition);
			g_currentSelectedEntity->m_taskQueue.push(task);
		}
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates resource
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resource::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	Entity::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Renders resource
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resource::Render()
{
	Entity::Render();
	std::string resourceName;
	switch (m_type)
	{
	case RESOURCE_FOOD:
		resourceName = "F";
		break;
	case RESOURCE_STONE:
		resourceName = "S";
		break;
	case RESOURCE_WOOD:
		resourceName = "W";
		break;
	default:
		break;
	}

	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, resourceName, g_fontSize, Rgba::YELLOW);
	delete textMaterial;
}
