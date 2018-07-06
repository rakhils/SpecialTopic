#include "Game/GamePlay/Entity/EnemyTank.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Physics/Terrain.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Debug/DebugDraw.hpp"

#include "Game/GamePlay/Entity/EnemyBase.hpp"
#include "Game/GamePlay/Entity/Bullet.hpp"
#include "Game/GamePlay/Scenes/SceneLevel1.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/Game.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EnemyTank::EnemyTank(std::string name, Scene *scene,Vector3 position) : GameObject(name)
{
	Mesh *turretHead = MeshBuilder::CreateUVSpehere<Vertex_3DPCUNTB>(Vector3::ZERO, 0.75f, 15, 15, Rgba::YELLOW, FILL_MODE_WIRE);
	Mesh *tankBody   = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3::ZERO, Vector3(1.f, .25f, 2.f), Rgba::WHITE, FILL_MODE_FILL);
	Mesh *turretGun  = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3::ZERO, Vector3(.1f, .1f, 2.f), Rgba::WHITE, FILL_MODE_FILL);
	GameObject *turretHeadGO = new GameObject("turrethead");
	GameObject *turretGunGO  = new GameObject("turretgun");
	
	turretHeadGO->SetScene(scene);
	turretHeadGO->m_renderable->SetMesh(turretHead);
	turretHeadGO->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));
	turretHeadGO->m_transform.SetLocalPosition(Vector3(0.f, 0.4f, 0.f));

	turretGunGO->SetScene(scene);
	turretGunGO->m_renderable->SetMesh(turretGun);
	turretGunGO->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));
	turretGunGO->m_transform.SetLocalPosition(Vector3(0.f, .5f, 3.f));

	SetScene(scene);
	m_renderable->SetMesh(tankBody);
	m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));

	AddChild(turretHeadGO);
	AddChild(turretGunGO);

	AddSphereCollider(Vector3::ZERO, 1.5f);

	scene->AddRenderable(m_renderable);
	scene->AddRenderable(turretGunGO->m_renderable);
	scene->AddRenderable(turretHeadGO->m_renderable);

	m_transform.SetLocalPosition(position);
	m_forward = Game::GetMaxBounds() / 2.f - m_transform.GetWorldPosition();
	m_forward.y = 0.f;
	m_forward = m_forward.GetNormalized();
}

// DESTRUCTOR
EnemyTank::~EnemyTank()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 EnemyTank::GetCurrentForward()
{
	return m_transform.GetWorldMatrix().GetKVector();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::Update(float deltatime)
{
	
	m_timeLeftForNextDirectionChange -= deltatime;
	Vector3 pos = m_transform.GetWorldPosition();
	//DebugDraw::GetInstance()->DebugRenderLogf("TANK POS %f %f %f", pos.x, pos.y, pos.z);
	m_forward = Vector3::ZERO;
	UpdateSeekDirection(45);
	UpdateSeperationDirection(250);
	UpdateCohesionDirection(1);
	UpdateAlignmentDirection(20);
	UpdateRandomDirection(1);

	UpdateTankTurn();
	UpdateTankOrientation();
	UpdateTankHeightOnTerrain();
	UpdateForceOnTank(deltatime);
	//UpdateHitFeedBack(deltatime);
	//UpdateBounds(deltatime);
	if (((SceneLevel1*)m_scene)->m_mode)
	{
		return;
	}
	DebugDraw::GetInstance()->DebugRenderLine(pos + Vector3(0,2,0), pos + m_forward * 10,Rgba::YELLOW);
	GameObject::Update(deltatime);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateHitFeedBack(float deltatime)
{
	if(m_hitInterval > 0)
	{
		m_hitInterval -= deltatime;
		ParticleEmitter *emitter = (ParticleEmitter*)GetComponentByType(PARTICLE);
		emitter->m_size = 5;
		emitter->SpawnParticles(4);
		emitter->Update(deltatime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateTankOrientation()
{
	Vector3 terrainNormal = ((SceneLevel1*)m_scene)->m_map->m_terrain->GetNormalAtXZ(m_transform.GetWorldPosition().GetXZ()).GetNormalized();
	Vector3 forward = m_transform.GetWorldMatrix().GetKVector().GetNormalized();

	Vector3 right = CrossProduct(terrainNormal, forward);
	right = right.GetNormalized();

	Vector3	correctedForward = CrossProduct(right, terrainNormal);

	Matrix44 matrix(Vector4(right.GetNormalized()), Vector4(terrainNormal.GetNormalized()), Vector4(correctedForward.GetNormalized()), Vector4(0, 0, 0, 1));
	//matrix.InvertFast1();
	Vector3 euler = matrix.GetEulerFromMatrix();
	m_transform.SetLocalRotationEuler(euler);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateTankHeightOnTerrain()
{
	Vector3 position = m_transform.GetWorldPosition();
	float   terrainHeight = ((SceneLevel1*)m_scene)->m_map->m_terrain->GetHeight(position.GetXZ());
	m_transform.SetLocalPosition(Vector3(position.x, terrainHeight + 1, position.z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateForceOnTank(float deltaTime)
{
	m_forward = m_forward.GetNormalized();
	if(DotProduct(m_forward,m_transform.GetWorldMatrix().GetKVector()) < 0.25)
	{
		return;
	}
	m_forward.y = 0;
	if(m_forward.x && m_forward.z == 0)
	{
		m_forward.x = 1;
		m_forward.z = 1;
	}
	GetRigidBody3DComponent()->AddForce(m_velocity*m_forward,deltaTime);
	m_forward = Vector3::ZERO;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateBounds(float deltaTime)
{
	Vector3 minBounds = Game::GetMinBounds();
	Vector3 maxBounds = Game::GetMaxBounds();
	Vector3 tankFuturePos = m_transform.GetWorldPosition() + (m_forward*((m_velocity)*deltaTime));
	if(tankFuturePos.x < minBounds.x || tankFuturePos.x > maxBounds.x)
	{
		ChangeForwardDirection(m_forward * -1);
	}
	if (tankFuturePos.z < minBounds.z || tankFuturePos.z > maxBounds.z)
	{
		ChangeForwardDirection(m_forward * -1);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateSeekDirection(float weight)
{
	Vector3 playerLocation = ((SceneLevel1*)m_scene)->m_playerTank->m_transform.GetWorldPosition();
	Vector3 distanceVec3   = playerLocation - m_transform.GetWorldPosition();
	if((distanceVec3).GetLength() < m_minDistanceToFollow)
	{
		m_forward += distanceVec3.GetNormalized() * weight;
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateSeperationDirection(float weight)
{
	Vector3 averageDirection;
	int     vehicleCount = 0;
	for (int index = 0; index < EnemyBase::s_enemyTanks.size(); index++)
	{
		if (EnemyBase::s_enemyTanks.at(index) == this)
		{
			continue;
		}
		EnemyTank *tank = EnemyBase::s_enemyTanks.at(index);
		Vector3 distance = tank->m_transform.GetWorldPosition() - m_transform.GetWorldPosition();
		if (distance.GetLength() < m_minDistanceForSeperation)
		{
			averageDirection += (distance.GetNormalized());
			vehicleCount++;
		}
	}
	if(vehicleCount <= 0)
	{
		return;
	}
	averageDirection /= static_cast<float>(vehicleCount);
	m_forward +=  averageDirection*(-1 * weight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateCohesionDirection(float weight)
{
	Vector3 averagePosition;
	int     vehicleCount = 0;
	for (int index = 0; index < EnemyBase::s_enemyTanks.size(); index++)
	{
		if (EnemyBase::s_enemyTanks.at(index) == this)
		{
			continue;
		}
		EnemyTank *tank = EnemyBase::s_enemyTanks.at(index);
		Vector3 distance = tank->m_transform.GetWorldPosition() - m_transform.GetWorldPosition();
		if (distance.GetLength() < m_minDistanceForCohesion)
		{
			averagePosition += tank->m_transform.GetWorldPosition();
			vehicleCount++;
		}
	}
	if (vehicleCount <= 0)
	{
		return;
	}
	averagePosition /= static_cast<float>(vehicleCount);
	Vector3 direction = averagePosition - m_transform.GetWorldPosition();
	m_forward += direction.GetNormalized() * weight;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateAlignmentDirection(float weight)
{
	Vector3 averageDirection;
	int     vehicleCount = 0;
	for (int index = 0; index < EnemyBase::s_enemyTanks.size(); index++)
	{
		if (EnemyBase::s_enemyTanks.at(index) == this)
		{
			continue;
		}
		EnemyTank *tank = EnemyBase::s_enemyTanks.at(index);
		Vector3 distance = tank->m_transform.GetWorldPosition() - m_transform.GetWorldPosition();
		if (distance.GetLength() < m_minDistanceForAlignment)
		{
			averageDirection += tank->GetCurrentForward();
			vehicleCount++;
		}
	}
	if (vehicleCount <= 0)
	{
		return;
	}
	averageDirection /= static_cast<float>(vehicleCount);
	m_forward += averageDirection * weight;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateRandomDirection(float weight)
{
	if(m_forward != Vector3::ZERO)
	{
		return;
	}
	if (m_timeLeftForNextDirectionChange <= 0)
	{
		m_seekPosition = GetRandomPoint(Vector3::ONE, Vector3::ONE * 512);
		m_timeLeftForNextDirectionChange = 5;
	}
	Vector3 direction = m_transform.GetWorldPosition() - m_seekPosition;
	m_forward = direction.GetNormalized()*weight;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateTankTurn()
{

	Matrix44 currentTransform = m_transform.GetLocalMatrix();
	Matrix44 targetTransform = Matrix44::LookAt(m_transform.GetLocalPosition(), m_transform.GetLocalPosition() + m_forward , Vector3::UP);
	//targetTransform.InvertFast1();
	//Vector3  targetAngle = targetTransform.GetEulerFromMatrix();

	float turn_this_frame = 5000;

	Matrix44 local = Matrix44::TurnTowards(currentTransform, targetTransform, turn_this_frame);
	m_transform.SetLocalMatrix(local);
	m_transform.SetLocalScale(Vector3(1, 1, 1));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::OnCollisionEnter(Collider *collider)
{
	GameObject *gameobject = collider->m_gameObject;
	if(Bullet *bullet = dynamic_cast<Bullet*>(gameobject) )
	{
		m_markAsDead = true;
	}
	if (Tank *pTank = dynamic_cast<Tank*>(gameobject))
	{
		m_markAsDead = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::ChangeForwardDirection(Vector3 direction)
{
	m_timeLeftForNextDirectionChange = 10.f;
	m_forward = direction;
}
