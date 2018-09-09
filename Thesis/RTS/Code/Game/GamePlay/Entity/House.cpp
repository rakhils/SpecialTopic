#include "Game/GamePlay/Entity/House.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"

// CONSTRUCTOR
House::House()
{

}

House::House(Map *map,Vector2 position, int teamID)
{
	m_map  = map;
	m_type = HOUSE;
	SetPosition(position);
	SetTeam(teamID);
}

// DESTRUCTOR
House::~House()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates house
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void House::Update(float deltaTime)
{
	Entity::Update(deltaTime);
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "H", g_fontSize, GetTeamColor());
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : render House
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void House::Render()
{
	Entity::Render();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "H", g_fontSize, GetTeamColor());
	delete textMaterial;
}
