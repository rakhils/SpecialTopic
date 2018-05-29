#pragma once
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Standalone curve utility functions
//
// Note: Quadratic Bezier functions (only) specify an actual midpoint, not a Bezier guide point
/////////////////////////////////////////////////////////////////////////////////////////////////

template< typename T >
T EvaluateQuadraticBezier( const T& startPos, const T& actualCurveMidPos, const T& endPos, float t );
 

template< typename T >
T EvaluateQuadraticBezier(const T& startPos, const T& actualCurveMidPos, const T& endPos, float t)
{
	float s = (1-t);
	const T& midpoint = (startPos + endPos )/2.0f;
	const T& lengthFromMidpoint = midpoint + (actualCurveMidPos - midpoint)*2.0f;

	return s*s*startPos + 2*s*t*lengthFromMidpoint + t*t*endPos;
}

template< typename T >
T EvaluateCubicBezier( const T& startPos, const T& guidePos1, const T& guidePos2, const T& endPos, float t );

template< typename T >
T EvaluateCubicBezier(const T& startPos, const T& guidePos1, const T& guidePos2, const T& endPos, float t)
{
	float e = Interpolate(startPos,guidePos1,t);
	float f = Interpolate(guidePos1,guidePos2,t);
	float g = Interpolate(guidePos2,endPos,t);

	float q = Interpolate(e,f,t);
	float r = Interpolate(f,g,t);
	
	float p = Interpolate(q,r,t);
	return p;
}

template< typename T >
T EvaluateCubicHermite( const T& startPos, const T& startVel, const T& endPos, const T& endVel, float t );


template< typename T >
T EvaluateCubicHermite(const T& startPos, const T& startVel, const T& endPos, const T& endVel, float t)
{
	float s = (1 - t);
	return s*s*(1 + 2*t)*startPos + t*t*(1+2*s)*endPos + s*s*t*startVel - s*t*t*endVel;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// CubicSpline2D
// 
// Cubic Hermite/Bezier spline of Vector2 positions / velocities
/////////////////////////////////////////////////////////////////////////////////////////////////
class CubicSpline2D
{
public:
	CubicSpline2D() {}
	explicit CubicSpline2D( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray=nullptr );
	~CubicSpline2D() {}

	// Mutators
	void		AppendPoint( const Vector2& position, const Vector2& velocity=Vector2(0,0) );
	void		AppendPoints( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray=nullptr );
	void		InsertPoint( int insertBeforeIndex, const Vector2& position, const Vector2& velocity=Vector2(0,0) );
	void		RemovePoint( int pointIndex );
	void		RemoveAllPoints();
	void		SetPoint( int pointIndex, const Vector2& newPosition, const Vector2& newVelocity );
	void		SetPosition( int pointIndex, const Vector2& newPosition );
	void		SetVelocity( int pointIndex, const Vector2& newVelocity );
	void		SetCardinalVelocities( float tension=0.f, const Vector2& startVelocity=Vector2(0,0), const Vector2& endVelocity=Vector2(0,0) );

	// Accessors
	int				GetNumPoints() const { return (int) m_positions.size(); }
	const Vector2	GetPosition( int pointIndex );
	const Vector2	GetVelocity( int pointIndex );
	int				GetPositions( std::vector<Vector2>& out_positions ) const;
	int				GetVelocities( std::vector<Vector2>& out_velocities ) const;
	Vector2			EvaluateAtCumulativeParametric( float t ) const;
	Vector2			EvaluateAtNormalizedParametric( float t ) const;

protected:
	std::vector<Vector2>	m_positions;
	std::vector<Vector2>	m_velocities;
};



