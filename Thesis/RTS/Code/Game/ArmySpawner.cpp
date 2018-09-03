#include "Game/ArmySpawner.hpp"

#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"

// CONSTRUCTOR
ArmySpawner::ArmySpawner()
{

}

ArmySpawner::ArmySpawner(Map *map, Vector2 position, int teamID)
{
	m_map = map;
	SetPosition(position);
	SetTeam(teamID);
}

// DESTRUCTOR
ArmySpawner::~ArmySpawner()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::Update(float deltaTime)
{
	Entity::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Renders army spawner
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::Render()
{
	Entity::Render();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "A", g_fontSize/2.f, GetTeamColor());
	delete textMaterial;
}
