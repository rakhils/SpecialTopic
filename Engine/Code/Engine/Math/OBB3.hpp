#pragma once
#include "Engine/Math/Vector3.hpp"
/*\class  : OBB3		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/12/2018 2:29:00 PM
* \contact: srsrakhil@gmail.com
*/
class OBB3
{

public:
	//Member_Variables
	Vector3 center;
	Vector3 right;
	Vector3 up;
	Vector3 forward;
	//Static_Member_Variables

	//Methods

	OBB3();
	~OBB3();
	
	bool DoContains(Vector3 point);
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