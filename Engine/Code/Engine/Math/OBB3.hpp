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
//   6 --- 7
//  /|    /|
// 2 +-- 3 |
// | 4 --| 5
// |/    |/
// 0 --- 1
class OBB3
{

public:
	//Member_Variables
	Vector3 m_center;
	Vector3 m_right;
	Vector3 m_up;
	Vector3 m_forward;
	//Static_Member_Variables

	//Methods

	OBB3();
	~OBB3();
	
	bool DoContainsPoint(Vector3 point);
	bool GetCorners(Vector3* out_corners);
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