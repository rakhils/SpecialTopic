#include "Engine/Math/OBB3.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtil.hpp"
// CONSTRUCTOR
OBB3::OBB3()
{

}

// DESTRUCTOR
OBB3::~OBB3()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : Checks if the given point is inside this obb3
*@param   : Point given
*@return  : true if point inside else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OBB3::DoContainsPoint(Vector3 point)
{
	Matrix44 spaceOrientation(m_right, m_up, m_forward, Vector4(0.f));
	spaceOrientation.Inverse();
	Vector3 local_pos = Matrix44::Multiply(Vector4(point, 1.0f), spaceOrientation).XYZ();
	return GetAbsolute(local_pos) <= Vector3::ONE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/18
*@purpose : retrieves corner points of obb3
*@param   : corner list
*@return  : If success true 
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OBB3::GetCorners(Vector3* out_corners)
{
	AABB3 aabb3 = AABB3::Center(Vector3::ZERO, Vector3(2.f));
	Matrix44 spaceOrientation(m_right, m_up, m_forward, Vector4(0.f));
	out_corners[0] = Matrix44::Multiply(Vector4(aabb3.GetBackBottomLeft  (), 0), spaceOrientation).XYZ();
	out_corners[1] = Matrix44::Multiply(Vector4(aabb3.GetBackBottomRight (), 0), spaceOrientation).XYZ();
	out_corners[2] = Matrix44::Multiply(Vector4(aabb3.GetBackTopLeft	 (), 0), spaceOrientation).XYZ();
	out_corners[3] = Matrix44::Multiply(Vector4(aabb3.GetBackTopRight    (), 0), spaceOrientation).XYZ();
	out_corners[4] = Matrix44::Multiply(Vector4(aabb3.GetFrontBottomLeft (), 0), spaceOrientation).XYZ();
	out_corners[5] = Matrix44::Multiply(Vector4(aabb3.GetFrontBottomRight(), 0), spaceOrientation).XYZ();
	out_corners[6] = Matrix44::Multiply(Vector4(aabb3.GetFrontTopLeft    (), 0), spaceOrientation).XYZ();
	out_corners[7] = Matrix44::Multiply(Vector4(aabb3.GetFrontTopRight   (), 0), spaceOrientation).XYZ();
	return true;
}
