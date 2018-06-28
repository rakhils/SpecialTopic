#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Game/GamePlay/Entity/EnemyTank.hpp"
#include "Game/GamePlay/Entity/Bullet.hpp"
// CONSTRUCTOR
Bullet::Bullet(std::string name,int team,Vector3 position,Vector3 direction,float speed): GameObject(name)
{
	m_teamNumber = team;
	m_forward = direction;
	m_transform.SetLocalPosition(position);
	MeshBuilder mb;
	MeshBuilder::CreateUVSpehere(mb,Vector3::ZERO, 0.35f, 8, 8, Rgba::RED, FILL_MODE_FILL);
	//MeshBuilder::Create3DPlane(mb, Vector3::ZERO, Vector3(0.5f, 0, 0), Vector3(0, 0.5f, 0), Vector2(m_radius, m_radius), Rgba::RED, FILL_MODE_FILL);
	//MeshBuilder::Create3DPlane(mb, Vector3::ZERO, Vector3(0, 0.5f, 0), Vector3(0, 0, 0.5f), Vector2(m_radius, m_radius), Rgba::RED, FILL_MODE_FILL);
	//MeshBuilder::Create3DPlane(mb, Vector3::ZERO, Vector3(0, 0, 0.5f), Vector3(0.5f, 0, 0), Vector2(m_radius, m_radius), Rgba::RED, FILL_MODE_FILL);
	Mesh *mesh = mb.CreateMesh();
	m_renderable->SetMesh(mesh);
	m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));
	m_renderable->m_material->m_textures.at(0) = Texture::GetDefaultTexture();

	m_speed = speed;
	AddSphereCollider(Vector3::ZERO, 1.f);
	AddLightComponent(Vector3::ZERO,Rgba::RED);
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
	//m_renderable->m_modelMatrix = m_transform.GetLocalMatrix();
	m_lifeTime -= deltaTime;
	m_transform.Translate(m_forward*m_speed);
	GameObject::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Bullet::OnCollisionEnter(Collider *collider)
{
	UNUSED(collider);
	if (EnemyTank *tank = dynamic_cast<EnemyTank*>(collider->m_gameObject))
	{
		m_markForDelete = true;
		m_lifeTime = 0.f;
	}
	
}
