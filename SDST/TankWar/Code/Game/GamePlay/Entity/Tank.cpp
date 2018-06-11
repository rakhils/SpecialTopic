#include "Game/GamePlay/Entity/Tank.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/SceneManagement/Scene.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Entity/Bullet.hpp"
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
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Tank::GetTurretForward()
{
	return Camera::GetGamePlayCamera()->GetCameraForwardVector();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::FireBullet(float deltaTime)
{
	Bullet *bullet = new Bullet("bullet_" + ToString(static_cast<int>(m_bullets.size())), m_transform.GetWorldPosition(), GetTurretForward(), 1);
	bullet->SetScene(m_scene);
	bullet->AddSphereCollider(Vector3::ZERO, bullet->m_radius);
	m_scene->AddRenderable(bullet->m_renderable);
	m_bullets.push_back(bullet);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::Update(float deltaTime)
{
	float PI = 3.14f;
	Vector3 minRotationValue(-PI / 2, 0, 0);
	Vector3 maxRotationValue(PI / 2, 2 * PI, 0);
	Vector2 delta = InputSystem::GetInstance()->GetMouseDelta();
	delta.y += .5;
	if (InputSystem::GetInstance()->WasLButtonJustPressed())
	{
		Vector2 screenXY = InputSystem::GetInstance()->GetMouseClientPosition();
		DebugDraw::GetInstance()->DebugRenderLogf("MOUSE XY %f, %f", screenXY.x, screenXY.y);
		PickRay ray = Camera::GetGamePlayCamera()->GetPickRayFromScreenCords(screenXY);
		DebugDraw::GetInstance()->DebugRenderLogf("MOUSESTART %f, %f, %f", ray.m_position.x,ray.m_position.y,ray.m_position.z);
		DebugDraw::GetInstance()->DebugRenderLogf("MOUSEDIRECTION %f, %f, %f", ray.m_direction.x, ray.m_direction.y, ray.m_direction.z);
		DebugRenderOptions options;
		Vector3 pos(ray.m_position.x,ray.m_position.y,ray.m_position.z);
		DebugDraw::GetInstance()->DebugRenderSphere(pos, 5, 16, 16, nullptr, Rgba::WHITE, DEBUG_RENDER_FILL, options);

		FireBullet(deltaTime);
		
	}
	if (delta.x != 0)
	{
		m_transform.RotateLocalByEuler(Vector3(0, 0.005f*delta.x, 0));
		Vector3 rotation = m_transform.GetLocalEulerAngles();
		rotation.y = fmodf(rotation.y, 2 * PI);
		m_transform.SetLocalRotationEuler(rotation);
	}




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
	UpdateBullet(deltaTime);
	GameObject::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::UpdateBullet(float deltaTime)
{
	for(int index = 0;index < m_bullets.size();index++)
	{
		Bullet *bullet = m_bullets.at(index);
		if(bullet->m_lifeTime < 0)
		{
			delete m_bullets.at(index);
			m_bullets.at(index) = nullptr;
			m_bullets.erase(m_bullets.begin() + index);
			index--;
			continue;
		}
		bullet->Update(deltaTime);
	}
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
