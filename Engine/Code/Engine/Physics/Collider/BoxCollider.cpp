#include "Engine/Physics/Collider/BoxCollider.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
BoxCollider::BoxCollider()
{

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
	UNUSED(deltaTime);
	std::map<std::string, GameObject*>::iterator itGameObject;
	for (itGameObject = GameObject::s_gameObjects.begin(); itGameObject != GameObject::s_gameObjects.end(); itGameObject++)
	{
		GameObject *gameObject = itGameObject->second;
		std::map<COMPONENT_COLLIDER_TYPE, Collider*>::iterator it;
		for (it = gameObject->m_colliderComponents.begin(); it != gameObject->m_colliderComponents.end(); it++)
		{

		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Checks if the point is inside boxcollider
*@param   : World Position
*@return  : return true if point is inside boxcollder else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BoxCollider::IsPointInside(Vector3 point)
{
	UNUSED(point);
	Vector3 centrePosition = GetCenterPosition();
	//Vector2 xMinMax(centrePosition.x - m_offsetX, centrePosition.x + m_offsetX);
	//Vector2 yMinMax(centrePosition.y - m_offsetY, centrePosition.y + m_offsetY);
	//Vector2 zMinMax(centrePosition.z - m_offsetZ, centrePosition.z + m_offsetZ);

	/*if (point.x > xMinMax.x && point.x < xMinMax.y)
	{
		if (point.y > yMinMax.x && point.y < yMinMax.y)
		{
			if (point.z > zMinMax.x && point.z < zMinMax.y)
			{
				return true;
			}
		}
	}*/
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
