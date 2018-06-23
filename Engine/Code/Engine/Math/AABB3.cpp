#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtil.hpp"
// CONSTRUCTOR
AABB3::AABB3()
{
	
}

AABB3::AABB3(Vector3 mins, Vector3 maxs)
{
	m_mins = mins;
	m_maxs = maxs;
}

// DESTRUCTOR
AABB3::~AABB3()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Retrieves back bottom left point
*@param   : NIL
*@return  : back bottom left point
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 AABB3::GetBackBottomLeft()
{
	Vector3 dimension = GetCentre() - m_mins;
	return (GetCentre() + Vector3(-dimension.x, -dimension.y, -dimension.z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : retrieves back bottom right
*@param   : NIL
*@return  : back bottom right point
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 AABB3::GetBackBottomRight()
{
	Vector3 dimension = GetCentre() - m_mins;
	return (GetCentre() + Vector3(dimension.x, -dimension.y, -dimension.z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : retrieves back top left
*@param   : NIL
*@return  : back top left point
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 AABB3::GetBackTopLeft()
{
	Vector3 dimension = GetCentre() - m_mins;
	return (GetCentre() + Vector3(-dimension.x, dimension.y, -dimension.z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Retrieves back top right 
*@param   : NIL
*@return  : back top right point
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 AABB3::GetBackTopRight()
{
	Vector3 dimension = GetCentre() - m_mins;
	return (GetCentre() + Vector3(dimension.x, dimension.y, -dimension.z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : retrieves front bottom left
*@param   : NIL
*@return  : front bottom left
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 AABB3::GetFrontBottomLeft()
{
	Vector3 dimension = GetCentre() - m_mins;
	return (GetCentre() + Vector3(-dimension.x, -dimension.y, dimension.z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Retrieves front bottom left
*@param   : NIL
*@return  : front bottom left
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 AABB3::GetFrontBottomRight()
{
	Vector3 dimension = GetCentre() - m_mins;
	return (GetCentre() + Vector3(dimension.x, -dimension.y, dimension.z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Retrieves front top left
*@param   : NIL
*@return  : Front top left
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 AABB3::GetFrontTopLeft()
{
	Vector3 dimension = GetCentre() - m_mins;
	return (GetCentre() + Vector3(-dimension.x, dimension.y, dimension.z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : retrieves front top right
*@param   : NIL
*@return  : front top right
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 AABB3::GetFrontTopRight()
{
	Vector3 dimension = GetCentre() - m_mins;
	return (GetCentre() + Vector3(dimension.x, dimension.y, dimension.z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Get center point
*@param   : NIL
*@return  : Center
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 AABB3::GetCentre()
{
	return (m_maxs + m_mins) / 2.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : Grows aabb3 to contain the given point
*@param   : Point to be considered
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABB3::GrowToContain(Vector3 point)
{
	m_mins = GetMin(point, m_mins);
	m_maxs = GetMin(point, m_maxs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : Checks whether point is inside aabb3
*@param   : Given point
*@return  : True if point inside else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABB3::DoContains(Vector3 point)
{
	if (point >= m_mins && point <= m_maxs)
	{
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : Retrieves all corners of aabb3
*@param   : array of corners to be retrieved
*@return  : If success in finding all corners
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABB3::GetCorners(Vector3 *out)
{
	out[0] = GetBackBottomLeft();
	out[1] = GetBackBottomRight();
	out[2] = GetBackTopLeft();
	out[3] = GetBackTopRight();
	out[4] = GetFrontBottomLeft(); 
	out[5] = GetFrontBottomRight();
	out[6] = GetFrontTopLeft();
	out[7] = GetFrontTopRight();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : Invalidates aabb3 by giving max value to m_mins and min value to m_maxs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABB3::Invalidate()
{
	m_mins = Vector3(INFINITY);
	m_maxs = Vector3(-INFINITY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Creates a new AABB3 with center and given dimensions
*@param   : Center position and dimension
*@return  : new AABB3
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AABB3 AABB3::Center(Vector3 center, Vector3 dimensions)
{
	Vector3 mins = center - dimensions/2.f;
	Vector3 maxs = center + dimensions/2.f;
	AABB3 aabb3(mins, maxs);
	return aabb3;
}