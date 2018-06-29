#pragma once
#include <vector>
#include "Engine/Core/Component.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Segment.hpp"
#include "Engine/Math/Plane.hpp"
/*\class  : Collider
* \group  : <GroupName>  
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 5/17/2018 4:18:53 PM
* \contact: srsrakhil@gmail.com
*/
class PointCollider;
class SphereCollider;
class BoxCollider2D;
class BoxCollider;
class AABB2Collider;
class CircleCollider;
enum COMPONENT_COLLIDER_TYPE
{
	POINT_COLLIDER,BOX_COLLIDER2D, BOX_COLLIDER, SPHERE_COLLIDER, CIRCLE_COLLIDER
};
class Collider : public Component
{

public:
	//Member_Variables
	COMPONENT_COLLIDER_TYPE m_colliderType;
	Vector3   m_centre;
	bool      m_isStatic  = false;
	bool	  m_doTrigger = false;
	typedef void (*collisionEnter)(Collider *collider);
	collisionEnter *m_funcCollisionEnter = nullptr;
	//Static_Member_Variables

	//Methods

	Collider();
	~Collider();
	
	Vector3 GetCenterPosition();
	void	SetPosition(Vector3 position);
	// ALL TYPE OF COLLISIONS
	bool CheckSphereVsPointCollision (SphereCollider* sphereCollider, PointCollider *point);
	bool CheckBoxVsPointCollision    (BoxCollider* sphereCollider, PointCollider *point);

	bool CheckSphereVsBox2DCollision (SphereCollider* sphereCollider, BoxCollider2D* boxCollider);
	bool CheckBox2DVsCircleCollision (BoxCollider2D * box2dCollider, CircleCollider* circleCollider,DIRECTIONS &area);
	bool CheckSphereVsSphereCollision(SphereCollider* sphereCollider1, SphereCollider* sphereCollider2);
	bool CheckSegmentVsPlane		 (Segment segment, Plane plane);
	void GlobalCollisionCheck();

	virtual void Update(float deltaTime) = 0;
	virtual bool IsPointInside(Vector3 point) = 0;
	virtual void OnCollisionEnter(Collider *collider) = 0;
	void         SetOnCollisionEnterCB(collisionEnter func);
	//Static_Methods
	std::string GetColliderNameFromType(COMPONENT_COLLIDER_TYPE type);
protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};