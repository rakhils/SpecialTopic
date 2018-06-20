#include "Engine/Physics/Collider/Collider.hpp"
#include "Engine/Physics/Collider/BoxCollider.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/Physics/Collider/SphereCollider.hpp"
#include "Engine/Physics/Collider/AABB2Collider.hpp"
#include "Engine/Physics/Collider/CircleCollider.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Physics/Raycast3D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Transform.hpp"
// CONSTRUCTOR
Collider::Collider()
{

}

// DESTRUCTOR
Collider::~Collider()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : returns the position
*@param   : NIL
*@return  : transform position
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Collider::GetCenterPosition()
{
	return m_transform.GetWorldPosition();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/26
*@purpose : Setns local  position of collider
*@param   : Local position
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Collider::SetPosition(Vector3 position)
{
	m_centre = position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Checks if sphere and box2d collision happend
*@param   : Sphere collider and box collider
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collider::CheckSphereVsBox2DCollision(SphereCollider* sphereCollider, BoxCollider2D* boxCollider)
{
	Vector3 sphereCenter = sphereCollider->GetCenterPosition();
	Vector3 direction    = sphereCenter - boxCollider->m_transform.GetWorldPosition();
	direction			 = direction.GetNormalized();
	RaycastHit result;// = Raycast3D::Raycast(sphereCollider, sphereCenter, direction*-1.f, radius, 0.10f);
	if(result.m_hit)
	{
		sphereCollider->OnCollisionEnter(boxCollider);
		//GameObject *gameObject = sphereCollider->m_gameObject;
		//gameObject->m_transform.Translate(box2DNormal*radius);

		/*Vector3 closestDirection;
		float dotValueX = GetModulus(DotProduct(direction, boxCollider->m_normalX.GetNormalized() * -1.f));
		float dotValueY = GetModulus(DotProduct(direction, boxCollider->m_normalY.GetNormalized() * -1.f));
		float dotValueZ = GetModulus(DotProduct(direction, boxCollider->m_normalZ.GetNormalized() * -1.f));
		
		if (dotValueX > dotValueY && dotValueX > dotValueZ)
		{
			closestDirection = boxCollider->m_normalX;
		}
		if (dotValueY > dotValueX && dotValueY > dotValueZ)
		{
			closestDirection = boxCollider->m_normalY;
		}
		if (dotValueZ > dotValueX && dotValueZ > dotValueY)
		{
			closestDirection = boxCollider->m_normalZ;
		}*/



		//RigidBody3D * rigidBody = gameObject->GetRigidBody3DComponent();
		TODO("DELTA TIME FOR SPHEREvsBOX COLLISION");
		//rigidBody->ApplyForce(closestDirection.GetNormalized()*(-1*rigidBody->m_mass*50.f), 0.016f);
		//boxCollider->OnCollisionEnter(sphereCollider);
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : Checks collision with AABB2 and Circle Collisions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collider::CheckBox2DVsCircleCollision(BoxCollider2D * box2dCollider, CircleCollider* circleCollider ,DIRECTIONS& area)
{
	Disc2 disk			   = circleCollider->m_disc;
	float radius		   = disk.radius;
	AABB2 aabb2			   = box2dCollider->m_aabb2;
	Vector2 distanceVector = Vector2(disk.center.x - aabb2.GetCenter().x, disk.center.y - aabb2.GetCenter().y);
	Vector2 diskPosition   = disk.center;
	Vector2 aabb2Position  = aabb2.GetCenter();

	if (disk.center.y > aabb2.mins.y && disk.center.y < aabb2.maxs.y)
	{
		//CHECK WEST
		if (disk.center.x < aabb2.mins.x)
		{
			if (disk.center.x + radius > aabb2.mins.x)
			{
				area = WEST;
				return true;
			}
		}
		//CHECK EAST
		if (disk.center.x > aabb2.maxs.x)
		{
			if (disk.center.x - radius < aabb2.maxs.x)
			{
				area = EAST;
				return true;
			}

		}
	}

	if (disk.center.x > aabb2.mins.x && disk.center.x < aabb2.maxs.x)
	{
		//CHECK NORTH
		if (disk.center.y > aabb2.maxs.y)
		{
			if (disk.center.y - radius < aabb2.maxs.y)
			{
				area = NORTH;
				return true;
			}
		}
		//CHECK SOUTH
		if (disk.center.y < aabb2.mins.y)
		{
			if (disk.center.y + radius > aabb2.mins.y)
			{
				area = SOUTH;
				return true;
			}
		}
	}

	if(disk.center.x < aabb2.mins.x)
	{
		//CHECK NORTH_WEST
		if (disk.center.y > aabb2.maxs.y)
		{
			Vector2 northWestCorner(aabb2.mins.x, aabb2.maxs.y);
			float distanceToCorner = (northWestCorner - disk.center).GetLength();
			if(distanceToCorner <= disk.radius)
			{
				area = NORTH_WEST;
				return true;
			}
		}
		//CHECK SOUTH_WEST
		if (disk.center.y < aabb2.mins.y)
		{
			Vector2 southWestCorner(aabb2.mins.x, aabb2.mins.y);
			float distanceToCorner = (southWestCorner - disk.center).GetLength();
			if (distanceToCorner <= disk.radius)
			{
				area = SOUTH_WEST;
				return true;
			}
		}
		// WEST
	}
	//CHECK EAST
	if (disk.center.x > aabb2.maxs.x)
	{
		//CHECK NORTH_EAST
		if (disk.center.y > aabb2.maxs.y)
		{
			Vector2 southWestCorner(aabb2.maxs.x, aabb2.maxs.y);
			float distanceToCorner = (southWestCorner - disk.center).GetLength();
			if (distanceToCorner <= disk.radius)
			{
				area = NORTH_EAST;
				return true;
			}
		}
		//CHECK SOUTH_EAST
		if (disk.center.y < aabb2.mins.y)
		{
			Vector2 southEastCorner(aabb2.maxs.x, aabb2.mins.y);
			float distanceToCorner = (southEastCorner - disk.center).GetLength();
			if (distanceToCorner <= disk.radius)
			{
				area = SOUTH_EAST;
				return true;
			}
		}
	}
	if (disk.center.x > aabb2.mins.x && disk.center.x < aabb2.maxs.x)
	{
		// ELSE CENTRE
		if (disk.center.y > aabb2.mins.y && disk.center.y < aabb2.maxs.y)
		{
			area = CENTRE;
			return true;
		}
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/13
*@purpose : Checks collision of sphere vs sphere
*@param   : Colliders
*@return  : True if collision exist
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collider::CheckSphereVsSphereCollision(SphereCollider* sphereCollider1, SphereCollider* sphereCollider2)
{
	Vector3 distance = sphereCollider1->GetCenterPosition() - sphereCollider2->GetCenterPosition();
	if(distance.GetLength() < sphereCollider1->m_radius + sphereCollider2->m_radius)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/18
*@purpose : Checks collision segment with plane
*@param   : Segment , plane
*@return  : if collides return true
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collider::CheckSegmentVsPlane(Segment segment, Plane plane)
{
	float d0 = plane.GetDistance(segment.m_start);
	float d1 = plane.GetDistance(segment.m_end);
	// different sign give negative output
	return (d0 * d1) <= 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : Checks all colliders collisions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Collider::GlobalCollisionCheck()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Sets the callback for collision enter
*@param   : function pointer
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Collider::SetOnCollisionEnterCB(collisionEnter func)
{
	m_funcCollisionEnter = &func;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : Returns the string version of collider type
*@param   : Collider type
*@return  : string representation 
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Collider::GetColliderNameFromType(COMPONENT_COLLIDER_TYPE type)
{
	switch (type)
	{
	case BOX_COLLIDER2D:
		return "BOX_COLLIDER2D";
		break;
	case BOX_COLLIDER:
		return "BOX_COLLIDER";
		break;
	case SPHERE_COLLIDER:
		return "SPHERE_COLLIDER";
		break;
	case CIRCLE_COLLIDER:
		return "CIRCLE_COLLIDER";
		break;
	default:
		break;
	}
	return "";
}
