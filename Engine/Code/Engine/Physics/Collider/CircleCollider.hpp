#pragma once
#include "Engine/Math/Disc2.hpp"
#include "Engine/Physics/Collider/Collider.hpp"
/*\class  : CircleCollider	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/8/2018 8:37:46 PM
* \contact: srsrakhil@gmail.com
*/
 
class CircleCollider : public Collider
{

public:
	//Member_Variables
	Disc2 m_disc;
	//Static_Member_Variables

	//Methods

	CircleCollider();
	~CircleCollider();
	
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