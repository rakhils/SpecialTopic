#include "Engine/Math/Frustum.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/AABB3.hpp"
// CONSTRUCTOR
Frustum::Frustum()
{

}

// DESTRUCTOR
Frustum::~Frustum()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : Check if frustrom contains the point
*@param   : Given point
*@return  : True if point is inside frustrum else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Frustum::DoContainPoint(Vector3 point)
{
	for(int i = 0;i < 6;i++)
	{
		if(!m_planes[i].IsInFront(point))
		{
			return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Creates frustum from matrix
*@param   : Given matrix
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Frustum Frustum::FromMatrix(Matrix44 matrix)
{
	AABB3 ndc = AABB3::Center(Vector3::ZERO, Vector3(2.0f));
	Vector3 corners[8];
	ndc.GetCorners(corners);
	matrix.Inverse();

	Vector3 world_corners[8];
	for (int cornerIndex = 0; cornerIndex < 8; cornerIndex++)
	{
		Vector4 ndc_pos            = Vector4(corners[cornerIndex], 1);
		Vector4 kinda_world_pos    = ndc_pos * matrix;
		world_corners[cornerIndex] = kinda_world_pos.XYZ() / kinda_world_pos.w;
	}

	Frustum frustum;
	frustum.m_left   = Plane(world_corners[0], world_corners[4], world_corners[2]);
	frustum.m_right  = Plane(world_corners[5], world_corners[1], world_corners[3]);
	frustum.m_bottom = Plane(world_corners[0], world_corners[1], world_corners[5]);
	frustum.m_top    = Plane(world_corners[2], world_corners[6], world_corners[7]);
	frustum.m_back   = Plane(world_corners[4], world_corners[5], world_corners[6]);
	frustum.m_front  = Plane(world_corners[1], world_corners[0], world_corners[2]);

	return frustum;
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Frustum Frustum::FromMatrix(Matrix44 mat)
{
	/ *AABB3 ndc = AABB3(Vector3 ZER0, 1, 1);
	Vector3 corners[8];
	ndc.GetCorners(corners);

	Matrix44 inverse = mat.Inverse();* /
}*/

