#pragma once
#include "Engine/Physics/Collider/Collider.hpp"
/*\class  : BoxCollider	   
* \group  : <GroupName>	   
* \brief  :	   
* \TODO:  :	   
* \note   :	   
* \author : Rakhil Soman	   
* \version: 1.0	   
* \date   : 5/17/2018 4:25:01 PM
* \contact: srsrakhil@gmail.com
*/
 
class BoxCollider : public Collider
{

public:
	//Member_Variables
	AABB2 m_aabb2;
	//Static_Member_Variables

	//Methods

	BoxCollider();
	~BoxCollider();

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