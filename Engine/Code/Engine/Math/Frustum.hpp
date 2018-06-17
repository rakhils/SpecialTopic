#pragma once
#pragma warning (disable : 4201)
#include "Engine/Math/Plane.hpp"
#include "Engine/Math/Vector3.hpp"

//#include "Engine/Math/Matrix44.hpp"
/*\class  : Frustum		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/12/2018 2:58:42 PM
* \contact: srsrakhil@gmail.com
*/
 
class Frustum
{

public:
	//Member_Variables
	/*Plane m_left;
	Plane m_right;
	Plane m_bottom;
	Plane m_top;
	Plane m_back;
	Plane m_front;*/

	union {
		Plane m_planes[6];
		struct {
			Plane m_left;
			Plane m_right;
			Plane m_bottom;
			Plane m_top;
			Plane m_back;
			Plane m_front;
		};
	};

	bool DoContainPoint(Vector3 point);
	//static Frustum FromMatrix(Matrix44 mat);
	//Static_Member_Variables

	//Methods

	Frustum();
	~Frustum();
	
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