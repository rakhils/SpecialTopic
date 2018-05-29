#include "Engine/Math/Disc2.hpp"

Disc2::Disc2( const Disc2& copyFrom )
{
	center = copyFrom.center;
	radius = copyFrom.radius;
}

Disc2::Disc2( float initialX, float initialY, float initialRadius )
{
	center.x = initialX;
	center.y = initialY;
	radius   = initialRadius;
}

Disc2::Disc2( const Vector2& initialCenter, float initialRadius )
{
	center = initialCenter;
	radius = initialRadius;
}

void Disc2::StretchToIncludePoint( float x, float y ) // expand radius if (x,y) is outside
{
	Vector2 newPointvector(x-center.x,y-center.y);
	float length = newPointvector.GetLength();
	if(length > radius)
	{
		radius = length;
	}
}

void Disc2::StretchToIncludePoint( const Vector2& point ) // expand radius if point outside
{
	StretchToIncludePoint(point.x,point.y);
}

void Disc2::AddPaddingToRadius( float paddingRadius )
{
	radius += paddingRadius;
}

void Disc2::Translate( const Vector2& translation ) // move the center
{
	center.x += translation.x;
	center.y += translation.y;
}

void Disc2::Translate( float translationX, float translationY ) // move the center
{
	center.x += translationX;
	center.y += translationY;
}

bool Disc2::IsPointInside( float x, float y ) const // is (x,y) within disc’s interior?
{
	Vector2 *pointVector = new Vector2(x-center.x,y-center.y);
	float length = pointVector->GetLength();
	if(length <= radius)
	{
		return true;
	}
	return false;
}

bool Disc2::IsPointInside( const Vector2& point ) const // is “point” within disc’s interior?
{
	return IsPointInside(point.x,point.y);
}

void Disc2::operator+=( const Vector2& translation ) // move
{
	center.x += translation.x;
	center.y += translation.y;
}
void Disc2::operator-=( const Vector2& antiTranslation )
{
	center.x -= antiTranslation.x;
	center.y -= antiTranslation.y;
}
Disc2 Disc2::operator+( const Vector2& translation ) const // create a moved copy
{
	float translationX = center.x + translation.x;
	float translationY = center.y + translation.y;
	Disc2 newDisc2(translationX,translationY,this->radius);
	return newDisc2;
}
Disc2 Disc2::operator-( const Vector2& antiTranslation ) const
{
	float translationX = center.x - antiTranslation.x;
	float translationY = center.y - antiTranslation.y;
	Disc2 newDisc2(translationX,translationY,this->radius);
	return newDisc2;
}
