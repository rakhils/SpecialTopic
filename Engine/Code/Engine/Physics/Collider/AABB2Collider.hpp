#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Physics/Collider/Collider.hpp"
/*\class  : AABB2Collider		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/8/2018 8:38:03 PM
* \contact: srsrakhil@gmail.com
*/
 
class AABB2Collider : public Collider
{

public:
	//Member_Variables
	AABB2 m_aabb2;
	//Static_Member_Variables

	//Methods

	AABB2Collider();
	~AABB2Collider();
	
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