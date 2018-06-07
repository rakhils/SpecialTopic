#include "Game/GamePlay/Entity/Tank.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
// CONSTRUCTOR
Tank::Tank() : GameObject("default")
{
	m_stopWatch.SetClock(Clock::g_theMasterClock);
	m_stopWatch.SetTimer(2);
}

// DESTRUCTOR
Tank::~Tank()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::Update(float deltaTime)
{
	DebugRenderOptions options;
	options.m_lifeTime = 4;
	options.m_startColor = Rgba::GREEN;
	options.m_endColor = Rgba::RED;
	m_stopWatch.Decrement();
	if (m_stopWatch.CheckAndReset())
	{
		Vector3 size(0.025f, 0.025f, 0.025f);
		DebugDraw::GetInstance()->DebugRenderCube(m_transform.GetWorldPosition(), size, nullptr, Rgba::WHITE, DEBUG_RENDER_FILL, options);
	}
	GameObject::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::OnCollisionEnter(Collider* collider)
{
	UNUSED(collider);
}
