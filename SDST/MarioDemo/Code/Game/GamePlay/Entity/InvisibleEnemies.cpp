#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GamePlay/Entity/InvisibleEnemies.hpp"
#include "Game/GameCommon.hpp"
// CONSTRUCTOR
InvisibleEnemies::InvisibleEnemies(std::string name,Vector2 position,float radius) : Entity(name) 
{
	SetPosition(position);
	AddCircleCollider(Vector3::ZERO, radius);
	m_radius = radius;
}

// DESTRUCTOR
InvisibleEnemies::~InvisibleEnemies()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InvisibleEnemies::Update(float deltaTime)
{
	if (g_theInput->wasKeyJustPressed(InputSystem::KEYBOARD_V))
	{
		m_visible = m_visible ? false : true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InvisibleEnemies::Render()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawCircle(m_transform.GetWorldPosition().GetXY(), m_radius);
	delete defaultMaterial;
}
