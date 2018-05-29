#pragma once
#include <string.h>

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix44.hpp"
 
//RANDOMG FLOAT , RANDOM INT
float		     GetRandomFloatInRange(float minInclusive, float maxExclusive);
float		     GetRandomFloatZeroToOne();
int			     GetRandomIntInRange(int minInclusive, int maxExclusive);
int			     GetRandomIntLessThan(int maxExclusive);
			     
// ANGLE , RADIANT RELATED FUNCTIONS
float		     GetRadiantFromDegree(float angle);
float		     ConvertRadiansToDegrees( float radians );
float		     ConvertDegreesToRadians( float degrees );
float		     ACosDegrees(float value);
float		     CosDegrees( float degrees );
float		     SinDegrees( float degrees );
float		     Atan2Degrees(float y,float x);
			     
float		     GetMaxOf2 (float value1, float value2);
float		     GetMinOf2 (float value1, float value2);
bool		     IsInRangeOf(int value,int lowerLimit,int upperLimit);
bool		     isInRangeOf(float value,float lowerLimit,float upperLimit);
float		     GetModulus(float value);
int			     GetModulus(int value);
			     
float		     GetDistance( const Vector2 a, const Vector2 b );
float		     GetDistanceSquared( const Vector2 a, const Vector2 b );
float		     DoAngleCorrection(float &angle);
bool		     IsAngleWithinRangeOf(float angle, float baseAngle, float range, float direction);
bool		     IsVectorAlmostEqual(Vector2 value1, Vector2 value2, float diff);
float		     GetLargestOf4(float value1, float value2, float value3, float value4);
float		     GetLargestOf8(float value1, float value2, float value3, float value4,float value5, float value6, float value7, float value8);

// DISK , AABB2 RELATED FUNCTINOS
bool			 DoAABBsOverlap(const AABB2& a, const AABB2& b);
bool			 DoDiscsOverlap(const Disc2& a, const Disc2& b);
bool			 DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius);
bool			 IsPointInsideAABB2(AABB2 aabb2, Vector2 position);
bool			 IsPointInsideDisc2(Vector2 pointValue,Disc2& disk);
bool			 DoDiscAndAABBOverlap(const AABB2 &aabb2, const Disc2& disk, Vector2 postionVector);
Vector2			 GetAABBDiscOverlapDistance(const AABB2& aabb2, const Disc2& disk,Vector2 directionVector,Vector2 positionVector);
				 
float			 turnTowards(float start, float end, float maxTurnDegrees);
float			 turnTowards1(float *start,float end,float maxDelta);
float			 TurnTowards2_oroginal( float currentDegrees, float goalDegrees, float maxTurnDegrees );
				 
float			 TurnTowards( float currentDegrees, float goalDegrees, float maxTurnDegrees );
float			 TurnTowardsInNegativeDirection(float m_angle,float m_finalAngle,float deltaAngle);
float			 TurnTowardsInPositiveDirection(float m_angle,float m_finalAngle,float deltaAngle);
float			 GetAngularDisplacement(float start,float end);
				 
Vector2			 Rotate2DVector(Vector2 direction, float angle);
Vector3			 RotateOnYAxis(Vector3 direction,float angle);
Vector3			 RotateOnZAxis(Vector3 direction,float angle);

// DOT PRODUCT
float			 DotProduct(Vector2 a, Vector2 b);
float			 DotProduct(Vector3 a, Vector3 b);
float			 DotProduct(Vector4 a, Vector4 b);
Vector3			 CrossProduct(Vector3 a,Vector3 b);

//PERCENTAGE RELATED FUNCTIONS
bool			 HasPercentChance ( float value);
bool			 CheckRandomChance( float chanceForSuccess );	// If 0.27f passed, returns true 27% of the time
int				 GetNextHigerMultiple(int currValue, int multipleValue);
int				 GetNextLowerMultiple(int currValue, int multipleValue);
				 
float			 RangeMapFloat(float value, float OriginalLowerBound, float OriginalUpperBound, float NewLowerbound, float NewUpperBound);
float			 RangeMapInt(int value, int OriginalLowerBound, int OriginalUpperBound, int NewLowerbound, int NewUpperBound);
float			 GetFractionInRange(float inValue, float rangeStart, float rangeEnd);
int				 RoundToNearestInt(float inValue);			// 0.5 rounds up to 1; -0.5 rounds up to 0
float			 ClampZeroOrToOne(float value);
int				 ClampInt( int inValue, int min, int max );
float			 ClampFloat( float inValue, float min, float max );
float			 ClampFloatZeroToOne( float inValue );
float			 ClampFloatNegativeOneToOne( float inValue );
Vector3			 ClampVector3(Vector3 inValue, Vector3 minClamp, Vector3 maxClamp);
bool			 Quadratic(Vector2 *out, float a, float b, float c);

// ADVANCED VECTOR FUNCTIONS/ PROJECTIONS AND REFLECTIONS
//////////////////////////////////////////////////////////////////////////
const Vector2    GetProjectedVector( const Vector2& vectorToProject, const Vector2& projectOnto );
// Returns the vector’s representation/coordinates in (i,j) space (instead of its original x,y space)
const Vector2    GetTransformedIntoBasis( const Vector2& originalVector,const Vector2& newBasisI, const Vector2& newBasisJ );
// Takes “vectorInBasis” in (i,j) space and returns the equivalent vector in [axis-aligned] (x,y) Cartesian space
const Vector2    GetTransformedOutOfBasis( const Vector2& vectorInBasis,const Vector2& oldBasisI, const Vector2& oldBasisJ );
// Decomposes “originalVector” into two component vectors, which add up to the original:
//   “vectorAlongI” is the vector portion in the “newBasisI” direction, and
//   “vectorAlongJ” is the vector portion in the “newBasisJ” direction.
void		     DecomposeVectorIntoBasis( const Vector2& originalVector,const Vector2& newBasisI, const Vector2& newBasisJ,
	Vector2& out_vectorAlongI, Vector2& out_vectorAlongJ );
const Vector2 Reflect(const Vector2& vectorToProject, const Vector2& projectOnto);
//////////////////////////////////////////////////////////////////////////

Vector2			 ConvertIsoToCartesian(Vector2 isoPoints);
Vector2			 ConvertCartesianToIso(Vector2 cartesianPoints);
Vector2			 ConvertPolarToCartesian(float radius, float angleInDegrees);
Vector2			 ConvertCartesianToPolar(float xPos, float yPos);
Vector3			 ConvertCartesianToSpherical(Vector3 position);
Vector3		     ConvertSphericalToCartesian(Vector3 position);
//////////////////////////////////////////////////////////////////////////


// BIT RELATED FUNCTIONS
bool			 AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck );
bool			 AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck );
void			 SetBits   ( unsigned char& bitFlags8, unsigned char flagsToSet );
void			 SetBits   ( unsigned int& bitFlags32, unsigned int flagsToSet );
void			 ClearBits ( unsigned char& bitFlags8, unsigned char flagToClear );
void			 ClearBits ( unsigned int& bitFlags32, unsigned int flagToClear );

//INTERPOLATE FUNCTIONS
int				 Interpolate( int start, int end, float fractionTowardEnd );
float			 Interpolate( float start, float end, float fractionTowardEnd );
unsigned char	 Interpolate( unsigned char start, unsigned char end, float fractionTowardEnd );
const IntVector2 Interpolate( const IntVector2& start, const IntVector2& end, float fractionTowardEnd );
const IntRange   Interpolate( const IntRange& start, const IntRange& end, float fractionTowardEnd );
const Rgba		 Interpolate( const Rgba& start, const Rgba& end, float fractionTowardEnd );
const Vector2    Interpolate( const Vector2& start, const Vector2& end, float fractionTowardEnd);
const Vector3    Interpolate( const Vector3& start, const Vector3& end, float fractionTowardEnd);
const Vector4    Interpolate( const Vector4& start, const Vector4& end, float fractionTowardEnd);
const FloatRange Interpolate( const FloatRange& start, const FloatRange& end, float fractionTowardEnd);
const AABB2      Interpolate( const AABB2& start, const AABB2& end, float fractionTowardEnd);
const Disc2      Interpolate( const Disc2& start, const Disc2& end, float fractionTowardEnd);

// EASING FUNCTIONS
float			 SmoothStart2(float t); // 2nd-degree smooth start (a.k.a. “quadratic ease in”)
float			 SmoothStart3(float t); // 3rd-degree smooth start (a.k.a. “cubic ease in”)
float			 SmoothStart4(float t); // 4th-degree smooth start (a.k.a. “quartic ease in”)
float			 SmoothStop2 (float t); // 2nd-degree smooth start (a.k.a. “quadratic ease out”)
float			 SmoothStop3 (float t); // 3rd-degree smooth start (a.k.a. “cubic ease out”)
float			 SmoothStop4 (float t); // 4th-degree smooth start (a.k.a. “quartic ease out”)
float			 SmoothStep3 (float t); // 3rd-degree smooth start/stop (a.k.a. “smoothstep”)
float			 CorssFade	 (float a, float b, float t);

