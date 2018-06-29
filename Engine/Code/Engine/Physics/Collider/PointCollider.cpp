#include "Engine/Physics/Collider/PointCollider.hpp"
#include "Engine/Physics/Collider/SphereCollider.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
PointCollider::PointCollider()
{

}

// DESTRUCTOR
PointCollider::~PointCollider()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PointCollider::Update(float deltaTime)
{
	UNUSED(deltaTime);
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
			if (it->first == SPHERE_COLLIDER)
			{
				bool result = CheckSphereVsPointCollision((SphereCollider*)it->second, this);
				if (result)
				{
					this->OnCollisionEnter((SphereCollider*)it->second);
					it->second->OnCollisionEnter(this);
				}
			}
			if (it->first == BOX_COLLIDER)
			{
				bool result = CheckBoxVsPointCollision((BoxCollider*)it->second, this);
				if (result)
				{
					this->OnCollisionEnter((SphereCollider*)it->second);
					it->second->OnCollisionEnter(this);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PointCollider::IsPointInside(Vector3 point)
{
	UNUSED(point);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PointCollider::OnCollisionEnter(Collider *collider)
{
	m_gameObject->OnCollisionEnter(collider);
}
