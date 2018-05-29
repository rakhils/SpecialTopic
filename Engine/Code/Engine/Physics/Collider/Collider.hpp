#pragma once
#include <vector>
#include "Engine/Core/Component.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vector3.hpp"
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
class SphereCollider;
class BoxCollider2D;
class BoxCollider;
class Collider : public Component
{

public:
	//Member_Variables
	Vector3   m_centre;
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
	bool CheckSphereVsBox2DCollision(SphereCollider* sphereCollider, BoxCollider2D* boxCollider);

	virtual void Update(float deltaTime) = 0;
	virtual bool IsPointInside(Vector3 point) = 0;
	virtual void OnCollisionEnter(Collider *collider) = 0;
	void         SetOnCollisionEnterCB(collisionEnter func);
	//Static_Methods

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