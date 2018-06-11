#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"

#include "Game/GamePlay/Entity/Bullet.hpp"
// CONSTRUCTOR
Bullet::Bullet(std::string name,Vector3 position,Vector3 direction,float speed): GameObject(name)
{
	m_forward = direction;
	MeshBuilder mb;
	MeshBuilder::Create3DPlane(mb, position, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector2(m_radius, m_radius), Rgba::RED, FILL_MODE_FILL);
	MeshBuilder::Create3DPlane(mb, position, Vector3(0, 1, 0), Vector3(0, 0, 1), Vector2(m_radius, m_radius), Rgba::RED, FILL_MODE_FILL);
	MeshBuilder::Create3DPlane(mb, position, Vector3(0, 0, 1), Vector3(1, 0, 0), Vector2(m_radius, m_radius), Rgba::RED, FILL_MODE_FILL);
	Mesh *mesh = mb.CreateMesh();
	m_renderable->SetMesh(mesh);
	m_renderable->SetMaterial(Material::AquireResource("default"));
	m_speed = speed;
}

// DESTRUCTOR
Bullet::~Bullet()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Bullet::Update(float deltaTime)
{
	m_renderable->m_modelMatrix = m_transform.GetLocalMatrix();
	m_lifeTime -= deltaTime;
	m_transform.Translate(m_forward*m_speed);
	GameObject::Update(deltaTime);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Bullet::Render()
{

}
