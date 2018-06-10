#pragma once
#include "Engine/Physics/Collider/Collider.hpp"
/*\class  : BoxCollider2D		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman	   
* \version: 1.0	   
* \date   : 5/18/2018 12:47:38 AM
* \contact: srsrakhil@gmail.com
*/
class BoxCollider2D : public Collider
{

public:
	//Member_Variables
	AABB2 m_aabb2;
	DIRECTIONS m_colliderDirection;
	//Static_Member_Variables

	//Methods

	BoxCollider2D();
	~BoxCollider2D();
	
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