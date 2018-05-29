#include "Engine/Math/AABB2.hpp"

AABB2::AABB2( const AABB2& copy )
{
	mins.x = copy.mins.x;
	mins.y = copy.mins.y;
	maxs.x = copy.maxs.x;
	maxs.y = copy.maxs.y;
}

AABB2::AABB2( float minX, float minY, float maxX, float maxY )
{
	this->mins.x = minX;
	this->mins.y = minY;
	this->maxs.x = maxX;
	this->maxs.y = maxY;
}

AABB2::AABB2( const Vector2& mins, const Vector2& maxs )
{
	this->mins.x = mins.x;
	this->mins.y = mins.y;
	this->maxs.x = maxs.x;
	this->maxs.y = maxs.y;
}

AABB2::AABB2( const Vector2& center, float radiusX, float radiusY )
{
	this->mins.x = center.x - radiusX;
	this->mins.y = center.y - radiusY;
	this->maxs.x = radiusX + center.x;
	this->maxs.y = radiusY + center.y;
}

void AABB2::StretchToIncludePoint( float x, float y ) // note: stretch, not move…
{
	if(x < mins.x)
	{
		mins.x = x;
	}
	if(x > maxs.x)
	{
		maxs.x = x;
	}
	if(y < mins.y)
	{
		mins.y = y;
	}
	if(y > maxs.y)
	{
		maxs.y = y;
	}
}

void AABB2::StretchToIncludePoint( const Vector2& point ) // note: stretch, not move…
{
	StretchToIncludePoint(point.x,point.y);
}

void AABB2::AddPaddingToSides( float xPaddingRadius, float yPaddingRadius )
{
	this->mins.x -= xPaddingRadius;
	this->maxs.x += xPaddingRadius;
	this->mins.y -= yPaddingRadius;
	this->maxs.y += yPaddingRadius;
}

void AABB2::Translate( const Vector2& translation ) // move the box; similar to +=
{
	this->mins.x += translation.x;
	this->mins.y += translation.y;
	this->maxs.x += translation.x;
	this->maxs.y += translation.y;
}

void AABB2::Translate( float translationX, float translationY )
{
	this->mins.x += translationX;
	this->mins.y += translationY;
	this->maxs.x += translationX;
	this->maxs.y += translationY;
}

bool AABB2::IsPointInside( float x, float y ) const // is “x,y” within box’s interior?
{
	if(mins.x < x  && maxs.x > x && mins.y < y  && maxs.y > y)
	{
		return true;
	}
	return false;
}

bool AABB2::IsPointInside( const Vector2& point ) const // is “point” within box’s interior?
{
	return IsPointInside(point.x,point.y);
}

Vector2 AABB2::GetDimensions() const // return a Vector2 of ( width, height )
{
	Vector2 dimensionVector(maxs.x-mins.x,maxs.y-mins.y);
	return dimensionVector;
}

Vector2 AABB2::GetCenter() const // return the center position of the box
{
	Vector2 CenterPosition(((maxs.x+mins.x)/2.0f),((maxs.y+mins.y)/2.0f));
	return CenterPosition;
}


//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/18
*@purpose : Interpolate AABB from start pos to end pos
*
*@param   : start AABB,end AABB, fraction towards the total interpolate
*
*@return  : Interpolated AABB at fractionTowardsEnd 
*/
//////////////////////////////////////////////////////////////
const AABB2 AABB2::Interpolate(const AABB2& start, const AABB2& end, float fractionTowardEnd)
{
	Vector2 finalMins = Vector2::Interpolate(start.mins,end.mins,fractionTowardEnd);
	Vector2 finalMaxs = Vector2::Interpolate(start.maxs,end.maxs,fractionTowardEnd);
	return AABB2(mins,maxs);
}

void AABB2::operator+=( const Vector2& translation ) // move (translate) the box
{
	this->mins.x += translation.x;
	this->mins.y += translation.y;
	this->maxs.x += translation.x;
	this->maxs.y += translation.y;
}

void AABB2::operator-=( const Vector2& antiTranslation )
{
	this->mins.x -= antiTranslation.x;
	this->mins.y -= antiTranslation.y;
	this->maxs.x -= antiTranslation.x;
	this->maxs.y -= antiTranslation.y;
}

AABB2 AABB2::operator-( const Vector2& antiTranslation ) const
{
	AABB2 aabb2(mins.x -antiTranslation.x,mins.y-antiTranslation.y,maxs.x-antiTranslation.x,maxs.y-antiTranslation.y);
	return aabb2;
}


AABB2 AABB2::operator+( const Vector2& translation ) const
{
	AABB2 aabb2(mins.x + translation.x,mins.y+translation.y,maxs.x+translation.x,maxs.y+translation.y);
	return aabb2;
}

