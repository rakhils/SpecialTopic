#pragma once
#include "Engine/Math/Vector3.hpp"
/*\class  : AABB3	   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/12/2018 2:21:19 PM
* \contact: srsrakhil@gmail.com
*/
 
class AABB3
{

public:
	//Member_Variables
	Vector3 mins;
	Vector3 maxs;
	//Static_Member_Variables
	//Methods

	AABB3();
	~AABB3();
	
	void GrowToContain(Vector3 point);
	bool DoContains(Vector3 point);
	bool GetCorners(Vector3 *out);
	void Invalidate();
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