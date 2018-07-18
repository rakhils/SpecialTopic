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
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/Camera/OrbitCamera.hpp"

#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/EnemyTank.hpp"
#include "Game/GamePlay/Entity/EnemyBase.hpp"
#include "Game/GamePlay/Scenes/SceneLevel1.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Entity/Bullet.hpp"
#include "Game/Game.hpp"
// CONSTRUCTOR
Tank::Tank() : GameObject("default")
{
	m_stopWatch.SetClock(Clock::g_theMasterClock);
	m_stopWatch.SetTimer(2);
	m_transform.SetLocalRotationEuler(Vector3::ZERO);
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
void Tank::FireBullet(float deltaTime,Vector3 direction,bool isArcher)
{
	if(m_bulletCoolDown > 0)
	{
		return;
	}
	AudioSystem::GetInstance()->PlaySoundFromGroup("shoot");
	UNUSED(deltaTime);
	Vector3 bulletSpawnPosition = m_turretHead->m_transform.GetWorldPosition();
	Bullet *bullet			    = new Bullet("bullet_" + ToString((m_bullets.size())),1, bulletSpawnPosition, direction, 1,isArcher);
	Light *light			    = (Light*)bullet->GetComponentByType(LIGHT);
	//LIGHT IS NULL IF IT EXCEED MAX LIMIT
	if(light != nullptr)
	{
		light->SetPointLightAttenutaion(Vector3(1, 1, 1));
		light->SetPointLigthSpecAttenuation(Vector3(1, 1, 1));
		m_scene->AddLight(light);
	}

	bullet->SetScene(m_scene);
	m_scene->AddRenderable(bullet->m_renderable);
	m_bullets.push_back(bullet);
	//m_bulletCoolDown = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Respawn at position where min distance from all enemy tanks and bases are min 5
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::Respawn(int currentRetryCount,int maxRetryCount)
{
	if (currentRetryCount > maxRetryCount)
	{
		Vector3 position = GetRandomPoint(Game::GetMinBounds(), Game::GetMaxBounds());
		m_health = 10;
		m_transform.SetLocalPosition(position);
		return;
	}
	Vector3 position = GetRandomPoint(Game::GetMinBounds(), Game::GetMaxBounds());
	for(int index = 0;index < EnemyBase::s_enemyTanks.size();index++)
	{
		Vector3 tankPosition = EnemyBase::s_enemyTanks.at(index)->m_transform.GetWorldPosition();
		Vector3 distance = tankPosition - position;
		if(distance.GetLength() < 5)
		{
			Respawn(++currentRetryCount,maxRetryCount);
			return;
		}
	}
	std::map<std::string, EnemyBase*>::iterator it;
	for(it = EnemyBase::s_enemyBases.begin();it != EnemyBase::s_enemyBases.end();it++)
	{

		Vector3 basePosition = it->second->m_transform.GetWorldPosition();
		Vector3 distance	 = position - basePosition;
		if(distance.GetLength() < 5)
		{
			Respawn(++currentRetryCount,maxRetryCount);
			return;
		}
	}
	m_health = 10;
	m_transform.SetLocalPosition(position);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::Update(float deltaTime)
{
	m_bulletCoolDown -= deltaTime;

	float PI = 3.14f;
	Vector3 minRotationValue(-PI / 2, 0, 0);
	Vector3 maxRotationValue(PI / 2, 2 * PI, 0);
	if(!((SceneLevel1*)m_scene)->m_mode)
	{
		if (InputSystem::GetInstance()->WasLButtonJustPressed())
		{
			FireBullet(deltaTime,m_turretForwardDirection,false);
		}
		if (InputSystem::GetInstance()->WasRButtonJustPressed())
		{
			FireBullet(deltaTime, m_turretForwardDirection, true);
		}
	}

	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_A))
	{
		Matrix44 worldMatrix = m_transform.GetWorldMatrix();
		Vector3 rigthDirection = worldMatrix.GetIVector();
		m_transform.Translate(rigthDirection*(-1 * deltaTime * m_velocity));
	}

	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_D))
	{
		Matrix44 worldMatrix = m_transform.GetWorldMatrix();
		Vector3 rigthDirection = worldMatrix.GetIVector();
		m_transform.Translate(rigthDirection*(1 * deltaTime* m_velocity));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_W))
	{
		Matrix44 worldMatrix = m_transform.GetWorldMatrix();
		Vector3 forwardDirection = worldMatrix.GetKVector();
		m_transform.Translate(forwardDirection*(1 * deltaTime * m_velocity));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_S))
	{
		Matrix44 worldMatrix = m_transform.GetWorldMatrix();
		Vector3 forwardDirection = worldMatrix.GetKVector();
		m_transform.Translate(forwardDirection*(-1 * deltaTime * m_velocity));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_Q))
	{
		m_transform.RotateLocalByEuler(Vector3(0, 5*deltaTime, 0));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_E))
	{
		m_transform.RotateLocalByEuler(Vector3(0, -5*deltaTime, 0));
	}

	Vector3 position		= m_transform.GetWorldPosition();
	float   terrainHeight   = ((SceneLevel1*)m_scene)->m_map->m_terrain->GetHeight(position.GetXZ());
	m_transform.SetLocalPosition(Vector3(position.x, terrainHeight + 1, position.z));

	ParticleEmitter *emitter = (ParticleEmitter*)GetComponentByType(PARTICLE);
	emitter->SpawnParticles(4);
	UpdateTankOrientation();
	UpdateRaycastFromTurret(deltaTime);
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
		float   terrainHeight   = ((SceneLevel1*)m_scene)->m_map->m_terrain->GetHeight(bullet->m_transform.GetWorldPosition().GetXZ());
		if(bullet->m_transform.GetWorldPosition().y < terrainHeight || bullet->m_lifeTime < 0 || bullet->m_markForDelete)
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
void Tank::UpdateTankOrientation()
{
	Vector3 terrainNormal    = ((SceneLevel1*)m_scene)->m_map->m_terrain->GetNormalAtXZ(m_transform.GetWorldPosition().GetXZ()).GetNormalized();
	Vector3 forward          = m_transform.GetWorldMatrix().GetKVector().GetNormalized();
	
	Vector3 right		     = CrossProduct(terrainNormal,forward);
	right					 = right.GetNormalized();

	Vector3	correctedForward = CrossProduct(right,terrainNormal);
	
	Matrix44 matrix(Vector4(right.GetNormalized()), Vector4(terrainNormal.GetNormalized()), Vector4(correctedForward.GetNormalized()), Vector4(0, 0, 0, 1));
	//matrix.InvertFast1();
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
	UNUSED(deltatime);
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
/*DATE    : 2018/06/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::UpdateRaycastFromTurret(float deltaTime)
{
	DebugRenderOptions options;
	options.m_lifeTime = 0;
	options.m_mode = DEBUG_RENDER_USE_DEPTH;
	Vector2 screenXY = InputSystem::GetInstance()->GetMouseClientPosition();
	if(((SceneLevel1*)m_scene)->m_mode)
	{
		screenXY = Windows::GetInstance()->GetDimensions().GetAsVector2() / 2.f;
	}
	PickRay ray = Camera::GetGamePlayCamera()->GetPickRayFromScreenCords(screenXY);

	//DebugDraw::GetInstance()->DebugRenderLogf("PICKRAY1 : RAYPOS :: %f, %f, %f", ray.m_position.x, ray.m_position.y, ray.m_position.z);
	//DebugDraw::GetInstance()->DebugRenderLogf("PICKRAY1 : RAYDIR :: %f, %f, %f", ray.m_direction.x, ray.m_direction.y, ray.m_direction.z);


	Ray raycast;
	raycast.m_direction = ray.m_direction;
	raycast.m_start = ray.m_position;// +((SceneLevel1*)m_scene)->m_cameraRadius*Camera::GetCurrentCamera()->m_transform.GetWorldMatrix().GetKVector();
	RaycastHit result = ((SceneLevel1*)m_scene)->m_map->m_terrain->Raycast(nullptr,raycast);


	raycast.m_direction = result.m_position - m_turretHead->m_transform.GetWorldPosition();
	raycast.m_direction = raycast.m_direction.GetNormalized();
	raycast.m_start     = m_turretHead->m_transform.GetWorldPosition();

	UpdateTurretOrientation1(deltaTime,raycast.m_direction);
	m_turretForwardDirection = raycast.m_direction;

	result = ((SceneLevel1*)m_scene)->m_map->m_terrain->Raycast(this,raycast);
	Rgba color;
	if(result.m_collider == nullptr)
	{
		color = Rgba::WHITE;
	}
	else
	{
		color = Rgba::RED;
	}
	m_mouseWorldPos = result.m_position;
	//DebugDraw::GetInstance()->DebugRenderLogf("RAYCAST : POSITION :: %f,%f %f", result.m_position.x, result.m_position.y, result.m_position.z);
	DebugDraw::GetInstance()->DebugRenderLine(raycast.m_start, result.m_position, Rgba::YELLOW, 0.f, DEBUG_RENDER_IGNORE_DEPTH);
	//DebugDraw::GetInstance()->DebugRenderLine(result.m_position, result.m_position + ray.m_direction * 1000, Rgba::RED, 0.f, DEBUG_RENDER_IGNORE_DEPTH);

	Vector3 cameraRight = Camera::GetCurrentCamera()->GetCameraRightVector();
	Vector3 cameraUp = Camera::GetCurrentCamera()->GetCameraUpVector();
	Vector2 terrainCellSize = ((SceneLevel1*)m_scene)->m_map->m_terrain->m_cellSize;
	terrainCellSize = terrainCellSize / 2.f;

	Vector3 distance = result.m_position - m_transform.GetWorldPosition();
	float quadSize = RangeMapFloat(distance.GetLength(), 0.f, 200.f, 0.2f, 1.f);
	options.m_startColor = options.m_endColor = color;
	DebugDraw::GetInstance()->DebugRenderQuad(result.m_position, AABB2(Vector2(0, 0), quadSize, quadSize), cameraRight, cameraUp, nullptr, color, DEBUG_RENDER_FILL, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::UpdateTurretOrientation(float deltaTime)
{
	Matrix44 worldMatrix   = m_transform.GetWorldMatrix();
	worldMatrix.Inverse();
	Vector3  localPosition    = Matrix44::Multiply(worldMatrix,Vector4(m_mouseWorldPos,1)).XYZ();
	Matrix44 targetTransform  = Matrix44::LookAt(m_turret->m_transform.GetLocalPosition(), localPosition, Vector3::UP);
	targetTransform.InvertFast1();
	m_turret->m_transform.SetLocalMatrix(targetTransform);
	//m_turret->m_transform.GetLocalMatrix().InvertFast();
	//m_turret->m_transform.SetLocalRotationEuler(m_turret->m_transform.GetLocalEulerAngles()*-1);

	m_turret->m_transform.SetLocalPosition(Vector3(0, 1, 0));
	Vector3 angle = m_turret->m_transform.GetWorldRotation();
	//DebugDraw::GetInstance()->DebugRenderLogf(Rgba::BLUE,"LocalPos %f, %f, %f", localPosition.x, localPosition.y, localPosition.z);
	//DebugDraw::GetInstance()->DebugRenderLogf(Rgba::YELLOW,"EULER ANGLE %f, %f, %f" , angle.x, angle.y, angle.z);
	UNUSED(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::UpdateTurretOrientation1(float deltaTime,Vector3 direction)
{
	UNUSED(direction);
	UNUSED(deltaTime);
	//m_turret->m_transform.SetLocalPosition(Vector3(0,0,0));
	Vector3 tankPosition = m_transform.GetWorldPosition();
	Vector3 tankUp = m_transform.GetWorldMatrix().GetKVector();
	Vector3 finalPosition = m_turretHead->m_transform.GetWorldPosition();


	Matrix44 worldMatrix = m_transform.GetWorldMatrix();
	worldMatrix.Inverse();
	Vector3  localPosition  = Matrix44::Multiply(worldMatrix,Vector4(m_mouseWorldPos, 1)).XYZ();
	Vector3  localPosition1 = Matrix44::Multiply(worldMatrix,Vector4(finalPosition, 1)).XYZ();

	Matrix44 currentTransform = m_turret->m_transform.GetLocalMatrix();
	Vector3  angleCurrent     = currentTransform.GetEulerFromMatrix();
	Matrix44 targetTransform  = Matrix44::LookAt(localPosition1, localPosition, Vector3::UP);

	float turn_this_frame = 1;

	Matrix44 local = Matrix44::TurnTowards(currentTransform, targetTransform, turn_this_frame);
	Vector3  targetAngle = local.GetEulerFromMatrix();

	m_turret->m_transform.SetLocalRotationEuler(targetAngle);

	Vector3 forward = ConvertSphericalToCartesian(targetAngle);

	Vector3 up      = m_transform.GetWorldMatrix().GetJVector();
	//Vector3 forward = m_turret->m_transform.GetWorldMatrix().GetKVector();

	//m_turret->m_transform.SetLocalPosition(up);

	//m_turret->m_transform.SetLocalPosition(up);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tank::OnCollisionEnter(Collider* collider)
{
	//can use template trick
	//getuniqueid for each Game object 
	if (EnemyTank *tank = dynamic_cast<EnemyTank*>(collider->m_gameObject)) // WONT GIVE INHERITANCE 
	{
		//m_health -= 1;
		if(m_health <=0)
		{
			m_markForDead = true;
			m_timeLeftToRespawn = 5.f;
		}
	}
}
