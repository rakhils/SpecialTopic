#include "Game/GamePlay/Entity/Tank.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Physics/Raycast3D.hpp"
#include "Engine/Physics/Raycast3D.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Physics/Terrain.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Game/GamePlay/Maps/Map.hpp"


#include "Game/GamePlay/Scenes/SceneLevel1.hpp"
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
	
	if (InputSystem::GetInstance()->IsLButtonDown())
	{
		Vector2 screenXY = InputSystem::GetInstance()->GetMouseClientPosition();
		DebugDraw::GetInstance()->DebugRenderLogf("MOUSE XY %f, %f", screenXY.x, screenXY.y);
		PickRay ray = Camera::GetGamePlayCamera()->GetPickRayFromScreenCords(screenXY);
		DebugDraw::GetInstance()->DebugRenderLogf("MOUSESTART %f, %f, %f", ray.m_position.x,ray.m_position.y,ray.m_position.z);
		DebugDraw::GetInstance()->DebugRenderLogf("MOUSEDIRECTION %f, %f, %f", ray.m_direction.x, ray.m_direction.y, ray.m_direction.z);
		DebugRenderOptions options;
		Vector3 pos(ray.m_position.x,ray.m_position.y,ray.m_position.z);
	
		Ray raycast;
		raycast.m_direction = ray.m_direction;
		raycast.m_start = m_transform.GetWorldPosition();
		m_mousePosForward = raycast.m_direction;


		//Collider *collider = Raycast3D::Raycast(this, raycast);
		//Raycast3D::Raycast()

		FireBullet(deltaTime);
	}

	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_A))
	{
		Matrix44 worldMatrix = m_transform.GetWorldMatrix();
		Vector3 rigthDirection = worldMatrix.GetIAxis();
		m_transform.Translate(rigthDirection*(-1 * deltaTime * m_velocity));
	}

	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_D))
	{
		Matrix44 worldMatrix = m_transform.GetWorldMatrix();
		Vector3 rigthDirection = worldMatrix.GetIAxis();
		m_transform.Translate(rigthDirection*(1 * deltaTime* m_velocity));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_W))
	{
		Matrix44 worldMatrix = m_transform.GetWorldMatrix();
		Vector3 forwardDirection = worldMatrix.GetKAxis();
		m_transform.Translate(forwardDirection*(1 * deltaTime * m_velocity));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_S))
	{
		Matrix44 worldMatrix = m_transform.GetWorldMatrix();
		Vector3 forwardDirection = worldMatrix.GetKAxis();
		m_transform.Translate(forwardDirection*(-1 * deltaTime * m_velocity));
	}

	Vector3 position		= m_transform.GetWorldPosition();
	float   terrainHeight   = ((SceneLevel1*)m_scene)->m_map->m_terrainOrig->GetHeight(position.GetXZ());
	m_transform.SetLocalPosition(Vector3(position.x, terrainHeight + 1, position.z));

	UpdateTurretOrientation(deltaTime);
	UpdateCameraPosition(deltaTime);
	UpdateCameraOrientation(deltaTime);
	UpdateTankOrientation();
	UpdateBreadCrumb(deltaTime);
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
		float   terrainHeight   = ((SceneLevel1*)m_scene)->m_map->m_terrainOrig->GetHeight(bullet->m_transform.GetWorldPosition().GetXZ());
		terrainHeight = 0;
		if(bullet->m_transform.GetWorldPosition().y < terrainHeight || bullet->m_lifeTime < 0)
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
/*DATE    : 2018/06/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::UpdateCameraPosition(float deltaTime)
{
	Vector3 tankPosition = m_transform.GetWorldPosition();
	Vector3 tankForward  = m_transform.GetWorldMatrix().GetKAxis();
	Vector3 camPosition  = tankPosition - tankForward * 25;
	PerspectiveCamera *m_camera = ((SceneLevel1*)m_scene)->m_camera;
	camPosition.y = 35;
	//m_camera->m_transform.SetLocalRotationEuler(Vector3(0, 45, 0));
	m_camera->m_transform.SetLocalPosition(camPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::UpdateCameraOrientation(float deltaTime)
{
	Vector2 delta = InputSystem::GetInstance()->GetMouseDelta();
	PerspectiveCamera *m_camera = ((SceneLevel1*)m_scene)->m_camera;
	if (delta.x != 0)
	{
		m_camera->m_transform.RotateLocalByEuler(Vector3(0, 0.005f*delta.x, 0));
		Vector3 rotation = m_camera->m_transform.GetLocalEulerAngles();
		rotation.y = fmodf(rotation.y, 2.f * 3.14f);
		m_camera->m_transform.SetLocalRotationEuler(rotation);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::UpdateTankOrientation()
{
	Vector3 terrainNormal = ((SceneLevel1*)m_scene)->m_map->m_terrainOrig->GetNormalAtXZ(m_transform.GetWorldPosition().GetXZ());
	Vector3 tangent       = ((SceneLevel1*)m_scene)->m_map->m_terrainOrig->GetTangentAtXZ(m_transform.GetWorldPosition().GetXZ());
	Vector3 bitangent     = CrossProduct(terrainNormal,tangent);

	Matrix44 matrix(Vector4(tangent.GetNormalized()), Vector4(terrainNormal.GetNormalized()), Vector4(bitangent.GetNormalized()), Vector4(0, 0, 0, 1));
	Vector3 euler = matrix.GetEulerFromMatrix();
	m_transform.SetLocalRotationEuler(euler);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::UpdateBreadCrumb(float deltatime)
{
	m_stopWatch.Decrement();
	if (m_stopWatch.CheckAndReset())
	{
		DebugRenderOptions options;
		options.m_lifeTime = 4;
		options.m_startColor = Rgba::GREEN;
		options.m_endColor = Rgba::RED;
		Vector3 size(0.025f, 0.025f, 0.025f);
		DebugDraw::GetInstance()->DebugRenderCube(m_transform.GetWorldPosition(), size, nullptr, Rgba::WHITE, DEBUG_RENDER_FILL, options);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::UpdateTurretOrientation(float deltaTime)
{

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
