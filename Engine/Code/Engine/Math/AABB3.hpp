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
//   6 --- 7
//  /|    /|
// 2 +-- 3 |
// | 4 --| 5
// |/    |/
// 0 --- 1
class AABB3
{

public:
	//Member_Variables
	Vector3 m_mins;
	Vector3 m_maxs;
	//Static_Member_Variables
	//Methods

	AABB3();
	AABB3(Vector3 mins, Vector3 maxs);
	~AABB3();
	
	// ALL CORNERS
	Vector3 GetBackBottomLeft();
	Vector3 GetBackBottomRight();
	Vector3 GetBackTopLeft();
	Vector3 GetBackTopRight();
	Vector3 GetFrontBottomLeft();
	Vector3 GetFrontBottomRight();
	Vector3 GetFrontTopLeft();
	Vector3 GetFrontTopRight();

	Vector3 GetCentre();

	void GrowToContain(Vector3 point);
	bool DoContains(Vector3 point);
	bool GetCorners(Vector3 *out);
	void Invalidate();
	//Static_Methods
	static AABB3 Center(Vector3 center, Vector3 dimensions);
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