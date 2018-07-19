#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Physics/Trajectory.hpp"

#include "Engine/Mesh/MeshBuilder.hpp"
#include "Game/GamePlay/Entity/EnemyTank.hpp"
#include "Game/GamePlay/Entity/Bullet.hpp"
#include "Game/GamePlay/Entity/EnemyBase.hpp"
#include "Engine/Debug/DebugDraw.hpp"
// CONSTRUCTOR
Bullet::Bullet(std::string name,int team,Vector3 position,Vector3 direction,float speed,bool isArcher,Vector3 endPos): GameObject(name)
{
	m_teamNumber = team;
	m_startPos = position;
	
	m_isTrajectory = isArcher;
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
	AddPointCollider(Vector3::ZERO);
	//AddSphereCollider(Vector3::ZERO, 1.f);
	AddLightComponent(Vector3::ZERO,Rgba::RED);


	if(isArcher)
	{
		Vector3 dist = endPos - position ;

		m_launchSpeed = Trajectory::GetMiniumLaunchVelocity(9.8f, dist.GetLength() + 10);
		Vector2 launchAngles;
		Trajectory::GetLaunchAngles(&launchAngles, 9.8f, m_launchSpeed, dist.GetLength() + 10);
		float angle = atan2(launchAngles.x, 1.f);
		m_launchAngle = angle;
		//m_launchAngle = GetMax(launchAngle.x, launchAngle.y);
		Vector3 posafterone = Trajectory::GetTrajectoryPositionAtTime(m_startPos, m_forward, m_launchSpeed, -9.8f, m_launchAngle, 1);
		m_newDirection = posafterone;// -position;
		//m_newDirection = m_newDirection.GetNormalized();
		
		for (float ttime = 0; ttime < 10.f; ttime += 0.016f)
		{
			Vector3 pos  = Trajectory::GetTrajectoryPositionAtTime(m_startPos, m_forward, m_launchSpeed, -9.8f, m_launchAngle, ttime);
			Vector3 pos1 = Trajectory::GetTrajectoryPositionAtTime(m_startPos, m_forward, m_launchSpeed, -9.8f, m_launchAngle, ttime + 0.016f);

			DebugDraw::GetInstance()->DebugRenderLine(pos, pos1,Rgba::WHITE,5);
		}


		/*AddRigidBody3DComponent();
		((RigidBody3D*)GetRigidBody3DComponent())->ApplyForce(direction * 7500.f,0.016f);
		((RigidBody3D*)GetRigidBody3DComponent())->m_gravity = Vector3(0, -9.8f, 0);
		((RigidBody3D*)GetRigidBody3DComponent())->m_useGravity = true;
		((RigidBody3D*)GetRigidBody3DComponent())->m_friction = 0.02f;*/
	}
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

	m_lifeTime -= deltaTime;
	if(m_isTrajectory)
	{
		m_startTime += deltaTime;
		Vector3 pos = Trajectory::GetTrajectoryPositionAtTime(m_startPos, m_forward, m_launchSpeed, -9.8f, m_launchAngle, m_startTime);
		//DebugDraw::GetInstance()->DebugRenderLogf("TRAJCET POS %f %f %f", pos.x, pos.y, pos.z);
		m_transform.SetLocalPosition(pos);
		GameObject::Update(deltaTime);
		return;
	}
	if((RigidBody3D*)GetRigidBody3DComponent() == nullptr)
	{
		m_transform.Translate(m_forward*m_speed);
	}
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
	if (EnemyBase *base = dynamic_cast<EnemyBase*>(collider->m_gameObject))
	{
		m_markForDelete = true;
		m_lifeTime = 0.f;
	}
	
}
