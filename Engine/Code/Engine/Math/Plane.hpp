#pragma once
#include "Engine/Math/Vector3.hpp"
/*\class  : Plane		   
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 6/12/2018 3:03:43 PM
* \contact: srsrakhil@gmail.com
*/
class Plane
{

public:
	//Member_Variables
	Vector3 m_normal;
	float   m_distance;
	//Static_Member_Variables

	//Methods

	Plane();
	Plane(Vector3 normal, float distance);
	Plane(Vector3 a, Vector3 b, Vector3 c);
	~Plane();
	
	float GetDistance(Vector3 point);
	bool  IsInFront(Vector3 point);
	void  FlipNormal();

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