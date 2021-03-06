#include "Engine/Physics/Collider/BoxCollider.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
BoxCollider::BoxCollider(Vector3 dimensions)
{
	m_aabb3.SetDimensions(dimensions);
}

// DESTRUCTOR
BoxCollider::~BoxCollider()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 BoxCollider::GetDimensions()
{
	return m_aabb3.GetDimensions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider::Update(float deltaTime)
{
	UNUSED(deltaTime);
	/*UNUSED(deltaTime);
	std::map<std::string, GameObject*>::iterator itGameObject;
	for (itGameObject = GameObject::s_gameObjects.begin(); itGameObject != GameObject::s_gameObjects.end(); itGameObject++)
	{
		GameObject *gameObject = itGameObject->second;
		std::map<COMPONENT_COLLIDER_TYPE, Collider*>::iterator it;
		for (it = gameObject->m_colliderComponents.begin(); it != gameObject->m_colliderComponents.end(); it++)
		{
			if (it->second == this)
			{
				continue;
			}
			if (it->first == BOX_COLLIDER2D)
			{
				CheckSphereVsBox2DCollision(this, (BoxCollider2D*)it->second);
			}
			if (it->first == SPHERE_COLLIDER)
			{
				bool result = CheckSphereVsSphereCollision(this, (SphereCollider*)it->second);
				if (result)
				{
					this->OnCollisionEnter((SphereCollider*)it->second);
					it->second->OnCollisionEnter(this);
				}
			}
		}
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Checks if the point is inside boxcollider
*@param   : World Position
*@return  : return true if point is inside boxcollder else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BoxCollider::IsPointInside(Vector3 point)
{
	Vector3 worldPosition = m_transform.GetWorldPosition();
	Vector3 dimensions    = m_aabb3.GetDimensions();
	if (point.x > worldPosition.x - dimensions.x/2 && point.x <  worldPosition.x + dimensions.x/2)
	{
		if (point.y >  worldPosition.y - dimensions.y/2 && point.y <  worldPosition.y + dimensions.y/2)
		{
			if (point.z >  worldPosition.z - dimensions.z/2 && point.z <  worldPosition.z + dimensions.z/2)
			{
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : on collision starts with other collider
*@param   : other collider
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider::OnCollisionEnter(Collider *collider)
{
	UNUSED(collider);
	m_gameObject->OnCollisionEnter(collider);
}
