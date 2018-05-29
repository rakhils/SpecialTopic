#pragma once
#include "Engine/Physics/Collider/Collider.hpp"
/*\class  : Sphere
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 5/17/2018 4:58:07 PM
* \contact: srsrakhil@gmail.com
*/
 
class SphereCollider : public Collider
{

public:
	//Member_Variables
	float				m_radius		= 1.f;
	//Static_Member_Variables

	//Methods

	SphereCollider();
	~SphereCollider();

	void Update(float deltaTime);
	bool IsPointInside(Vector3 point);
	void OnCollisionEnter(Collider *collider);
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