#include "Game/GamePlay/Entity/DebugEntity.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugEntity::Render()
{
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	Renderer::GetInstance()->DrawTextOn3DPoint(m_disc.center, Vector3::RIGHT, Vector3::UP, m_debugString.c_str(), m_fontSize, color);
	delete textMaterial;
}

// CONSTRUCTOR
DebugEntity::DebugEntity()
{
	m_startTime = static_cast<float>(GetCurrentTimeSeconds());
}

// DESTRUCTOR
DebugEntity::~DebugEntity()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/30
*@purpose : UYpdates the entity
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugEntity::Update(float deltaTime)
{
	UNUSED(deltaTime);
	if(static_cast<float>(GetCurrentTimeSeconds()) > m_startTime + 60)
	{
		m_isCompleted = true;
	}
}
