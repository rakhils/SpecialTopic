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
/*DATE    : 2018/05/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider::Update(float deltaTime)
{
	//UNUSED(deltaTime);
	//std::map<std::string, GameObject*>::iterator itGameObject;
	//for (itGameObject = GameObject::s_gameObjects.begin(); itGameObject != GameObject::s_gameObjects.end(); itGameObject++)
	//{
	//	GameObject *gameObject = itGameObject->second;
	//	std::map<COMPONENT_COLLIDER_TYPE, Collider*>::iterator it;
	//	for (it = gameObject->m_colliderComponents.begin(); it != gameObject->m_colliderComponents.end(); it++)
	//	{
	//
	//	}
	//}
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
	if (point.x > worldPosition.x - m_aabb3.m_mins.x && point.x <  worldPosition.x + m_aabb3.m_maxs.x)
	{
		if (point.y >  worldPosition.y - m_aabb3.m_mins.y && point.y <  worldPosition.y + m_aabb3.m_maxs.y)
		{
			if (point.z >  worldPosition.z - m_aabb3.m_mins.z && point.z <  worldPosition.z + m_aabb3.m_maxs.z)
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
}
