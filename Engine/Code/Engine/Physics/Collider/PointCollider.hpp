#pragma once
#include <map>
#include "Engine/Physics/Collider/Collider.hpp"
/*\class  : PointCollider		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/28/2018 3:46:26 PM
* \contact: srsrakhil@gmail.com
*/
 
class PointCollider : public Collider
{

public:
	//Member_Variables
	Vector3 m_point;
	//Static_Member_Variables

	//Methods

	PointCollider();
	~PointCollider();
	
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