#include "Game/GamePlay/Entity/EnemyBase.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Game/GamePlay/Entity/EnemyTank.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::map<std::string, EnemyBase*> EnemyBase::s_enemyBases;
EnemyBase::EnemyBase(std::string name, Vector3 position,Vector3 dimensions) : GameObject(name)
{
	MeshBuilder mb;
	MeshBuilder::CreateCube(mb, position, dimensions, Rgba::WHITE, FILL_MODE_FILL);
	Mesh *mesh = mb.CreateMesh();
	m_renderable->SetMesh(mesh);
	m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));

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
	std::string name(m_name + "tank_" + ToString(m_tanks.size()));
	EnemyTank *enemyTank = new EnemyTank(name, scene, m_transform.GetWorldPosition());
	m_tanks.push_back(enemyTank);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyBase::Update(float deltaTime)
{
	for(int enemyTankIndex = 0;enemyTankIndex < m_tanks.size();enemyTankIndex++)
	{
		m_tanks.at(enemyTankIndex)->Update(deltaTime);
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
		it->second->Update(deltaTime);
	}
}
