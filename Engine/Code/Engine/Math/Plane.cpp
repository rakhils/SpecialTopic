#include "Engine/Math/Plane.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
// CONSTRUCTOR
Plane::Plane()
{

}

Plane::Plane(Vector3 normal, float distance)
{
	m_normal   = normal;
	m_distance = distance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Construct plane from 3 points(min 3 points needed to contruct plane in 3D)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Plane::Plane(Vector3 a, Vector3 b, Vector3 c)
{
	Vector3 e0 = b - a;
	Vector3 e1 = c - a;

	m_normal = CrossProduct(e1, e0);
	//ASSERT(!IsNearZero(m_normal.GetLength()));

	m_normal   = m_normal.GetNormalized();
	m_distance = DotProduct(m_normal, a);
}

// DESTRUCTOR
Plane::~Plane()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : retrieves the distance from given point
*@param   : point
*@return  : distance
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Plane::GetDistance(Vector3 point)
{
	float  distance    = DotProduct(point, m_normal);
	return distance    - m_distance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : checks if the point is in front of the plane (positive distance direction)
*@param   : NIL
*@return  : true if is in front else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Plane::IsInFront(Vector3 point)
{
	if(GetDistance(point) > 0.f)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Flips the normal(puts plane's face in the opposite direction)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Plane::FlipNormal()
{
	m_normal   = -1 * m_normal;
	m_distance = -1 * m_distance;
}
