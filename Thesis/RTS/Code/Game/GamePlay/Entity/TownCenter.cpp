#include "Game/GamePlay/Entity/TownCenter.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Task/TaskSpawnVillager.hpp"
// CONSTRUCTOR
TownCenter::TownCenter()
{

}

TownCenter::TownCenter(Map *map,Vector2 position, int teamId) 
{
	m_map   = map;
	m_type	= TOWN_CENTER;
	m_teamID = teamId;
	SetPosition(position);
}

// DESTRUCTOR
TownCenter::~TownCenter()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Recevies all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::ProcessInputs(float deltaTime)
{
	UNUSED(deltaTime);
	if (g_currentSelectedEntity == this)
	{
		if(InputSystem::GetInstance()->WasLButtonJustPressed())
		{
			Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
			mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
			if (g_unitStatHUDFirstButton.IsPointInside(mousePosition))
			{
				Task *task = new TaskSpawnVillager(m_map, this);
				m_taskQueue.push(task);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates TownCenter
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	Entity::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Render TownCenter
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::Render()
{
	Entity::Render();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "T", g_fontSize, GetTeamColor());
	delete textMaterial;
}
