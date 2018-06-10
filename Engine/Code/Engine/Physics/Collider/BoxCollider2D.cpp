#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
BoxCollider2D::BoxCollider2D()
{

}

// DESTRUCTOR
BoxCollider2D::~BoxCollider2D()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider2D::Update(float deltaTime)
{
	UNUSED(deltaTime);
	// IF STATIC IT WONT COLLIDE WITH ANY OTHER OBJECTS. OTHER OBJECTS COLLIDE WITH THIS
	if(m_isStatic) { return;}

	std::map<std::string, GameObject*>::iterator itGameObject;
	for (itGameObject = GameObject::s_gameObjects.begin(); itGameObject != GameObject::s_gameObjects.end(); itGameObject++)
	{
		GameObject *gameObject = itGameObject->second;
		if(gameObject == m_gameObject)
		{
			continue;
		}
		std::map<COMPONENT_COLLIDER_TYPE, Collider*>::iterator it;
		for (it = gameObject->m_colliderComponents.begin(); it != gameObject->m_colliderComponents.end(); it++)
		{
			Collider *collider = it->second;
			if(collider->m_colliderType == CIRCLE_COLLIDER)
			{
				DIRECTIONS areaOfContact;
				if(CheckBox2DVsCircleCollision(this,(CircleCollider*)collider,areaOfContact))
				{
					collider->OnCollisionEnter(this);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Checks if point is inside box collider
*@param   : world position
*@return  : return true of point inside box2d collder else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BoxCollider2D::IsPointInside(Vector3 point)
{
	if (point.GetXY().x > m_aabb2.mins.x && point.GetXY().x < m_aabb2.maxs.x)
	{
		if (point.GetXY().y > m_aabb2.mins.y && point.GetXY().y < m_aabb2.maxs.y)
		{
			return true;
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
void BoxCollider2D::OnCollisionEnter(Collider *collider)
{
	m_gameObject->OnCollisionEnter(collider);
}
