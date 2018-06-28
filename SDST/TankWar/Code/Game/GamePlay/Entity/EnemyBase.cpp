#include "Game/GamePlay/Entity/EnemyBase.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/DebugDraw.hpp"

#include "Game/GamePlay/Entity/Tank.hpp"
#include "Game/GamePlay/Entity/Bullet.hpp"
#include "Game/GamePlay/Entity/EnemyTank.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::map<std::string, EnemyBase*> EnemyBase::s_enemyBases;
std::vector<EnemyTank*> EnemyBase::s_enemyTanks;
EnemyBase::EnemyBase(std::string name, Vector3 position,Vector3 dimensions) : GameObject(name)
{
	m_transform.SetLocalPosition(position);
	MeshBuilder mb;
	MeshBuilder::CreateCube(mb, Vector3::ZERO, dimensions, Rgba::WHITE, FILL_MODE_FILL);
	Mesh *mesh = mb.CreateMesh();
	m_renderable->SetMesh(mesh);
	m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));
	AddBoxCollider(Vector3::ZERO, dimensions.x);
}

// DESTRUCTOR
EnemyBase::~EnemyBase()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyBase::SpawnEnemy(Scene *scene)
{
	std::string name(m_name + "tank_" + ToString(s_enemyTanks.size()));
	Vector3 position = m_transform.GetWorldPosition();
	float randomX = GetRandomFloatInRange(130, 160);
	float randomZ = GetRandomFloatInRange(130, 160);

	EnemyTank *enemyTank = new EnemyTank(name, scene,Vector3(randomX,10,randomZ) );
	enemyTank->AddRigidBody3DComponent();
	s_enemyTanks.push_back(enemyTank);
	std::string text("Enemy Spawned at " + m_name +" TANK NUM "+name);
	DebugDraw::GetInstance()->DebugRenderLogf(2,text.c_str());
	DebugDraw::GetInstance()->DebugRenderLogf(2,"POSITION %f,%f,%f",position.x,position.y,position.z);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyBase::Update(float deltaTime)
{
	m_currentTime += deltaTime;
	if(m_currentTime > m_spawnInterval)
	{
		if(m_maxChildCount > 0)
		{
			SpawnEnemy(m_scene);
			m_maxChildCount--;
		}
		m_currentTime = 0.f;
	}

	GameObject::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyBase::OnCollisionEnter(Collider *collider)
{
	if(Tank *tank = dynamic_cast<Tank*>(collider->m_gameObject))
	{
		m_health--;
	}
	if (Bullet *bullet = dynamic_cast<Bullet*>(collider->m_gameObject))
	{
		m_health--;
		//std::string text(m_name + "HIT !! HEALTH LEFT %d ");
		DebugDraw::GetInstance()->DebugRenderLogf(3, "ENEMY BASE HIT!! HEALTH LEFT %f", m_health);
	}
	if(m_health <0)
	{
		m_markForDelete = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyBase::AddEnemyBase(std::string name, Vector3 position, Vector3 dimensions,Scene *scene)
{
	EnemyBase *enemyBase = new EnemyBase(name, position, dimensions);
	enemyBase->SetScene(scene);
	s_enemyBases[name] = enemyBase;
	scene->AddRenderable(enemyBase->m_renderable);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyBase::CreateEnemy(std::string baseName,Scene *scene)
{
	std::map<std::string, EnemyBase*>::iterator it;
	it = s_enemyBases.find(baseName);
	if(it!= s_enemyBases.end())
	{
		it->second->SpawnEnemy(scene);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyBase::UpdateEnemyBase(float deltaTime)
{
	std::map<std::string, EnemyBase*>::iterator it;
	for(it = s_enemyBases.begin(); it != s_enemyBases.end();it++)
	{
		if(it->second->m_markForDelete)
		{
			delete it->second;
			it->second = nullptr;
			s_enemyBases.erase(it);
			break;
		}
		it->second->Update(deltaTime);
	}
	UpdateEnemyTanks(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyBase::UpdateEnemyTanks(float deltatime)
{
	for(int index = 0;index < s_enemyTanks.size();index++)
	{
		if (s_enemyTanks.at(index)->m_markAsDead)
		{
			delete s_enemyTanks.at(index);
			s_enemyTanks.at(index) = nullptr;
			s_enemyTanks.erase(s_enemyTanks.begin() + index, s_enemyTanks.begin() + index + 1);
			index--;
			continue;
		}
		s_enemyTanks.at(index)->Update(deltatime);
	}
}
