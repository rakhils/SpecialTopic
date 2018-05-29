#include "Engine/Physics/Collider/Collider.hpp"
#include "Engine/Physics/Collider/BoxCollider.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/Physics/Collider/SphereCollider.hpp"
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
	float   radius		 = sphereCollider->m_radius;
	Vector3 direction    = sphereCenter - boxCollider->m_transform.GetWorldPosition();
	direction			 = direction.GetNormalized();
	RaycastHit result    = Raycast3D::Raycast(sphereCollider,sphereCenter, direction*-1.f, radius, 0.10f);
	if(result.m_hit)
	{
		sphereCollider->OnCollisionEnter(boxCollider);
		GameObject *gameObject = sphereCollider->m_gameObject;
		//gameObject->m_transform.Translate(box2DNormal*radius);

		Vector3 closestDirection;
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
		}



		RigidBody3D * rigidBody = gameObject->GetRigidBody3DComponent();
		TODO("DELTA TIME FOR SPHEREvsBOX COLLISION");
		rigidBody->ApplyForce(closestDirection.GetNormalized()*(-1*rigidBody->m_mass*50.f), 0.016f);
		//boxCollider->OnCollisionEnter(sphereCollider);
		return true;
	}
	return false;
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
