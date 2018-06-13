#include "Engine/Physics/Collider/SphereCollider.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
SphereCollider::SphereCollider()
{

}

// DESTRUCTOR
SphereCollider::~SphereCollider()
{
	int a = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SphereCollider::Update(float deltaTime)
{
	UNUSED(deltaTime);
	std::map<std::string, GameObject*>::iterator itGameObject;
	for (itGameObject = GameObject::s_gameObjects.begin(); itGameObject != GameObject::s_gameObjects.end(); itGameObject++)
	{
		GameObject *gameObject = itGameObject->second;
		std::map<COMPONENT_COLLIDER_TYPE, Collider*>::iterator it;
		for (it = gameObject->m_colliderComponents.begin(); it != gameObject->m_colliderComponents.end(); it++)
		{
			if(it->second == this)
			{
				continue;
			}
			if(it->first == BOX_COLLIDER2D)
			{
				CheckSphereVsBox2DCollision(this, (BoxCollider2D*)it->second);
			}
			if(it->first == SPHERE_COLLIDER)
			{
				bool result = CheckSphereVsSphereCollision(this, (SphereCollider*)it->second);
				if(result)
				{
					this->OnCollisionEnter((SphereCollider*)it->second);
					it->second->OnCollisionEnter(this);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Checks if the point is inside the sphere collider
*@param   : WorldPosition
*@return  : true if point inside sphere else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SphereCollider::IsPointInside(Vector3 point)
{
	Vector3 distance = point - GetCenterPosition();
	if(distance.GetLength() < m_radius)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : On Collision just starts
*@param   : the other collider which collides with this
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SphereCollider::OnCollisionEnter(Collider *collider)
{
	//Vector3 direction = m_gameObject->GetWorldPosition() - collider->GetCenterPosition();
	//direction         = direction.GetNormalized();
	//m_gameObject->GetRigidBody3DComponent()->ApplyForce(direction*10, 0.016f);
	


	m_gameObject->OnCollisionEnter(collider);
}
