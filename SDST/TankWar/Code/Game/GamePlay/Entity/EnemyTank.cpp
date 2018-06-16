#include "Game/GamePlay/Entity/EnemyTank.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Physics/Terrain.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"

#include "Game/GamePlay/Entity/Bullet.hpp"
#include "Game/GamePlay/Scenes/SceneLevel1.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EnemyTank::EnemyTank(std::string name, Scene *scene,Vector3 position) : GameObject(name)
{
	position += Vector3::ONE*40;
	position.y = 20;
	Mesh *turretHead = MeshBuilder::CreateUVSpehere<Vertex_3DPCUNTB>(Vector3::ZERO, 0.75f, 15, 15, Rgba::YELLOW, FILL_MODE_WIRE);
	Mesh *tankBody   = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3::ZERO, Vector3(1, .25, 2), Rgba::WHITE, FILL_MODE_FILL);
	Mesh *turretGun  = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3::ZERO, Vector3(.1, .1, 3), Rgba::WHITE, FILL_MODE_FILL);
	GameObject *turretHeadGO = new GameObject("turrethead");
	GameObject *turretGunGO  = new GameObject("turretgun");
	
	turretHeadGO->SetScene(scene);
	turretHeadGO->m_renderable->SetMesh(turretHead);
	turretHeadGO->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));
	turretHeadGO->m_transform.SetLocalPosition(Vector3(0, 0.4, 0));

	turretGunGO->SetScene(scene);
	turretGunGO->m_renderable->SetMesh(turretGun);
	turretGunGO->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));
	turretGunGO->m_transform.SetLocalPosition(Vector3(0, .5, 3));

	SetScene(scene);
	m_renderable->SetMesh(tankBody);
	m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));

	AddChild(turretHeadGO);
	AddChild(turretGunGO);

	AddSphereCollider(Vector3::ZERO, 1.5f);

	scene->AddRenderable(m_renderable);
	scene->AddRenderable(turretGunGO->m_renderable);
	scene->AddRenderable(turretHeadGO->m_renderable);

	m_transform.SetLocalPosition(Vector3(129, 23, 277));
	AddParticleComponent(Vector3::ZERO, Renderer::GetInstance());
}

// DESTRUCTOR
EnemyTank::~EnemyTank()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::Update(float deltatime)
{
	//m_transform.SetLocalPosition(Vector3(129, 23, 277));
	//m_transform.Translate(Vector3::FORWARD*(deltatime * 3));
	Vector3 playerPosition  = ((SceneLevel1*)m_scene)->m_playerTank->m_transform.GetWorldPosition();
	Vector3 distance = playerPosition - m_transform.GetWorldPosition();
	if(distance.GetLength() < m_distanceToFollow)
	{
		UpdateFollowBehaviour(deltatime);
	}
	else
	{
		UpdateWanderBehaviour(deltatime);
	}
	UpdateTankOrientation();
	UpdateHitFeedBack(deltatime);
	Vector3 position	  = m_transform.GetWorldPosition();
	float   terrainHeight = ((SceneLevel1*)m_scene)->m_map->m_terrainOrig->GetHeight(position.GetXZ());
	m_transform.SetLocalPosition(Vector3(position.x, terrainHeight + 1, position.z));


	GameObject::Update(deltatime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateWanderBehaviour(float deltaTime)
{
	float random = GetRandomFloatZeroToOne();
	if(random > 0.95)
	{
		m_forward = GetRandomDirection();
	}
	m_forward.y = 0;
	m_transform.Translate(m_forward*deltaTime);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyTank::UpdateFollowBehaviour(float deltaTime)
{
	Vector3 playerPosition  = ((SceneLevel1*)m_scene)->m_playerTank->m_transform.GetWorldPosition();
	m_forward				= playerPosition - m_transform.GetWorldPosition();
	m_forward.y = 0;
	m_transform.Translate(m_forward*deltaTime);
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
	Vector3 terrainNormal = ((SceneLevel1*)m_scene)->m_map->m_terrainOrig->GetNormalAtXZ(m_transform.GetWorldPosition().GetXZ());
	Vector3 tangent		  = ((SceneLevel1*)m_scene)->m_map->m_terrainOrig->GetTangentAtXZ(m_transform.GetWorldPosition().GetXZ());
	Vector3 bitangent	  = CrossProduct(terrainNormal, tangent);

	Matrix44 matrix(Vector4(tangent.GetNormalized()), Vector4(terrainNormal.GetNormalized()), Vector4(bitangent.GetNormalized()), Vector4(0, 0, 0, 1));
	Vector3 euler = matrix.GetEulerFromMatrix();
	m_transform.SetLocalRotationEuler(euler);
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
		m_hitInterval = 2.f;
	}
}
