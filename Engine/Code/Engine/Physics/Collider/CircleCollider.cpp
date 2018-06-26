#include "Engine/Physics/Collider/CircleCollider.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
// CONSTRUCTOR
CircleCollider::CircleCollider()
{

}

// DESTRUCTOR
CircleCollider::~CircleCollider()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : Updates collider
*@param   : delta
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CircleCollider::Update(float deltaTime)
{
	UNUSED(deltaTime);
	// IF STATIC IT WONT COLLIDE WITH ANY OTHER OBJECTS. OTHER OBJECTS COLLIDE WITH THIS
	if (m_isStatic) { return; }

	m_disc.center = m_transform.GetWorldPosition().GetXY();

	std::map<std::string, GameObject*>::iterator itGameObject;
	for (itGameObject = GameObject::s_gameObjects.begin(); itGameObject != GameObject::s_gameObjects.end(); itGameObject++)
	{
		GameObject *gameObject = itGameObject->second;
		if (gameObject == m_gameObject)
		{
			continue;
		}
		std::map<COMPONENT_COLLIDER_TYPE, Collider*>::iterator it;
		for (it = gameObject->m_colliderComponents.begin(); it != gameObject->m_colliderComponents.end(); it++)
		{
			Collider *collider = it->second;
			if (collider->m_colliderType == BOX_COLLIDER2D)
			{
				DIRECTIONS areaOfContact;
				if (CheckBox2DVsCircleCollision((BoxCollider2D*)collider,this,areaOfContact))
				{
					((BoxCollider2D*)collider)->m_colliderDirection = areaOfContact;
					OnCollisionEnter(collider);
					//DebugDraw::GetInstance()->DebugRenderLogf("HIT12234   %i",(int)areaOfContact);
					//DebugDraw::GetInstance()->DebugRenderLogf("HITTT "+GetDirectionNameFromType(areaOfContact),"");
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : Checks if a point is inside the collider
*@param   : Point in world
*@return  : true of world point is inside collider else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CircleCollider::IsPointInside(Vector3 point)
{
	if((point.GetXY() - m_disc.center).GetLength() < m_disc.radius)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CircleCollider::OnCollisionEnter(Collider *collider)
{
	m_gameObject->OnCollisionEnter(collider);
}
