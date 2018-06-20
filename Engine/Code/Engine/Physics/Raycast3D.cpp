#include "Engine/Physics/Raycast3D.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Physics/Collider/Collider.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
Raycast3D::Raycast3D()
{

}

// DESTRUCTOR
Raycast3D::~Raycast3D()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Raycast in 3D and fetch results
*@param   : Collider, raycast startpoint,direction, max distance until it reach to check, delta length-> step length
*@return  : RaycastHit result telling hit true of false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RaycastHit Raycast3D::Raycast(GameObject *currentGameObject,Ray ray)
{
	Vector3 startPosition = ray.m_start;
	Vector3 direction	  = ray.m_direction;
	float   maxDistance   = ray.m_maxDistance;
	float   deltaLength   = ray.m_stepSize;
	RaycastHit raycastHit;
	if(direction == Vector3::ZERO)
	{
		raycastHit.m_hit = false;
		return raycastHit;
	}
	Vector3 distance(Vector3::ZERO);
	for(int step = 0;distance.GetLength() < maxDistance;step++)
	{
		distance              = direction * (static_cast<float>(step) * deltaLength);
		Vector3 worldPosition = startPosition + distance;
		
		std::map<std::string, GameObject*>::iterator itGameObject;
		for (itGameObject = GameObject::s_gameObjects.begin(); itGameObject != GameObject::s_gameObjects.end(); itGameObject++)
		{
			GameObject *gameObject = itGameObject->second;
			if(gameObject == currentGameObject)
			{
				continue;
			}
			std::map<COMPONENT_COLLIDER_TYPE, Collider*>::iterator it;
			for (it = gameObject->m_colliderComponents.begin(); it != gameObject->m_colliderComponents.end(); it++)
			{
				if (it->second->IsPointInside(worldPosition))
				{
					raycastHit.m_position = worldPosition;
					raycastHit.m_collider = it->second;
					raycastHit.m_hit	  = true;

					DebugRenderOptions options;
					TODO("Debug draws in raycast");
					DebugDraw::GetInstance()->SetDebugRender3DCamera(Camera::GetGamePlayCamera());
					DebugDraw::GetInstance()->m_renderer = Renderer::GetInstance();
					DebugDraw::GetInstance()->DebugRenderLineSegment(startPosition, Rgba::RED, startPosition + direction * static_cast<float>(step), Rgba::RED, options);
					DebugDraw::GetInstance()->DebugRenderWireSphere(startPosition +direction*static_cast<float>(step),1,options);
					return raycastHit;
				}
			}
		}
	}
	DebugRenderOptions options;
	DebugDraw::GetInstance()->SetDebugRender3DCamera(Camera::GetGamePlayCamera());
	DebugDraw::GetInstance()->m_renderer = Renderer::GetInstance();
	DebugDraw::GetInstance()->DebugRenderLineSegment(startPosition, Rgba::RED,startPosition + direction*maxDistance,Rgba::RED,options);
	DebugDraw::GetInstance()->DebugRenderWireSphere(startPosition + direction*maxDistance, 1, options);
	raycastHit.m_hit = false;
	return raycastHit;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : get the collider at position
*@param   : position and direction heading
*@return  : Collider
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Collider* Raycast3D::GetColliderAtPosition(GameObject *currentGameobject ,Vector3 position)
{
	std::map<std::string, GameObject*>::iterator itGameObject;
	for (itGameObject = GameObject::s_gameObjects.begin(); itGameObject != GameObject::s_gameObjects.end(); itGameObject++)
	{
		GameObject *gameObject = itGameObject->second;
		if(gameObject == currentGameobject)
		{
			continue;
		}
		std::map<COMPONENT_COLLIDER_TYPE, Collider*>::iterator it;
		for (it = gameObject->m_colliderComponents.begin(); it != gameObject->m_colliderComponents.end(); it++)
		{
			if (it->second->IsPointInside(position))
			{
				return it->second;
			}
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Check whether ray hits plane
*@param   : Ray , plane object
*@return  : Raycast hit result
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RaycastHit Raycast3D::RaycastOnPlane(Ray ray, Plane plane)
{
	RaycastHit resultHit;
	float velocity = DotProduct(ray.m_direction, plane.m_normal);
	if (IsNearZero(velocity)) 
	{
		resultHit.m_hit = false;
		return resultHit;
	}

	float direction = DotProduct(plane.m_normal, ray.m_direction);
	float dist	    = plane.GetDistance(ray.m_start);
	if ((direction * dist) > 0.0f) 
	{
		resultHit.m_hit = false;
		return resultHit;
	}

	float time		= -dist / velocity;
	resultHit.m_hit = true;
	resultHit.m_position = ray.Evaluate(time);
	return resultHit;
}
