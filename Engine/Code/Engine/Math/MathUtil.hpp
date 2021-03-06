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

enum DIRECTIONS
{
	NORTH_WEST,NORTH,NORTH_EAST,   WEST,CENTRE,EAST,   SOUTH_WEST,SOUTH,SOUTH_EAST
};
//RANDOMG FLOAT , RANDOM INT
float		     GetRandomFloatInRange(float minInclusive, float maxExclusive);
float		     GetRandomFloatZeroToOne();
int			     GetRandomIntInRange(int minInclusive, int maxExclusive);
int			     GetRandomIntLessThan(int maxExclusive);
char			 GetRandomCharacter();
Vector3			 GetRandomDirection();
Vector3			 GetRandomPoint(Vector3 minBounds, Vector3 maxBounds);
bool			 GetRandomBoolean();

double			 GetRandomDoubleInRange(float minInclusive, float maxExclusive);

// ANGLE , RADIANT RELATED FUNCTIONS
float		     GetRadiantFromDegree(float angle);
float		     ConvertRadiansToDegrees( float radians );
float		     ConvertDegreesToRadians( float degrees );
float		     ACosDegrees(float value);
float		     CosDegrees( float degrees );
float		     SinDegrees( float degrees );
float		     Atan2Degrees(float y,float x);
			     
Vector2			 GetMin(Vector2 a, Vector2 b);
Vector3			 GetMin(Vector3 a, Vector3 b);
Vector2			 GetMax(Vector2 a, Vector2 b);
Vector3			 GetMax(Vector3 a, Vector3 b);
int				 GetMax(int a, int b);
uint16_t		 GetMax(uint16_t a, uint16_t b);
float			 GetMax(float a, float b);
float		     GetMaxOf2 (float value1, float value2);
float		     GetMinOf2 (float value1, float value2);
double			 GetMin(double value1, double value2);
int				 GetMin(int value1, int value2);
bool		     IsInRangeOf(int value,int lowerLimit,int upperLimit);
bool		     isInRangeOf(float value,float lowerLimit,float upperLimit);
float		     GetAbsolute(float value);
int			     GetAbsolute(int value);
Vector2			 GetAbsolute(Vector2 value);
Vector3          GetAbsolute(Vector3 value);
Vector4			 GetAbsolute(Vector4 value);
double			 GetPower(int value, int power);
bool			 CycleLess(uint16_t value1, uint16_t value2);
bool			 CycleGreater(uint16_t value1, uint16_t value2);
			     
float		     GetDistance( const Vector2 a, const Vector2 b );
float		     GetDistanceSquared( const Vector2 a, const Vector2 b );
float		     DoAngleCorrection(float &angle);
bool		     IsAngleWithinRangeOf(float angle, float baseAngle, float range, float direction);
bool			 IsAlmostEqual(float value1, float comparison, float diff = 0.001);
bool			 IsNearZero(float value);
bool		     IsVectorAlmostEqual(Vector2 value1, Vector2 value2, float diff);
bool			 IsAlmostEqual(Vector3 value1, Vector3 comparion, float diff = 0.001);
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
float			 GetFraction(float value);
int				 LoopValue(int value, int lowerBound, int upperBound);

Vector2			 RangeMap(Vector2 value, Vector2 originalLowerBound, Vector2 originalUpperBound, Vector2 newLowerBound, Vector2 newUpperBound);
Vector3          RangeMap(Vector3 value, Vector3 originalLowerBound, Vector3 originalUppperBound, Vector3 newLowerBound, Vector3 newUpperBound);
float			 RangeMapFloat(float value, float OriginalLowerBound, float OriginalUpperBound, float NewLowerbound, float NewUpperBound);
double			 RangeMap(double value, double OriginalLowerBound, double OriginalUpperBound, double NewLowerbound, double NewUpperBound);
float			 RangeMapInt(int value, int OriginalLowerBound, int OriginalUpperBound, int NewLowerbound, int NewUpperBound);
float			 GetFractionInRange(float inValue, float rangeStart, float rangeEnd);
int				 RoundToNearestInt(float inValue);			// 0.5 rounds up to 1; -0.5 rounds up to 0
int				 GetFlooredValue(float value);
float			 ClampZeroOrToOne(float value);
int				 ClampInt( int inValue, int min, int max );
float			 ClampFloat( float inValue, float min, float max );
double			 ClampDouble(double inValue, double min, double max);
float			 ClampFloatZeroToOne( float inValue );
float			 ClampFloatNegativeOneToOne( float inValue );
Vector3			 ClampVector3(Vector3 inValue, Vector3 minClamp, Vector3 maxClamp);
Vector2			 ClampVector2(Vector2 inValue, Vector2 minClamp, Vector2 maxClamp);
bool			 Quadratic(Vector2 *out, float a, float b, float c);
void			 SetRandomSRAND();
// ADVANCED VECTOR FUNCTIONS/ PROJECTIONS AND REFLECTIONS
//////////////////////////////////////////////////////////////////////////
const Vector2    GetProjectedVector( const Vector2& vectorToProject, const Vector2& projectOnto );
// Returns the vector�s representation/coordinates in (i,j) space (instead of its original x,y space)
const Vector2    GetTransformedIntoBasis( const Vector2& originalVector,const Vector2& newBasisI, const Vector2& newBasisJ );
// Takes �vectorInBasis� in (i,j) space and returns the equivalent vector in [axis-aligned] (x,y) Cartesian space
const Vector2    GetTransformedOutOfBasis( const Vector2& vectorInBasis,const Vector2& oldBasisI, const Vector2& oldBasisJ );
// Decomposes �originalVector� into two component vectors, which add up to the original:
//   �vectorAlongI� is the vector portion in the �newBasisI� direction, and
//   �vectorAlongJ� is the vector portion in the �newBasisJ� direction.
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
// NUMBER SYSTEM
std::string      GetHexaDecimalEquivalent(int value);

// BIT RELATED FUNCTIONS
bool			 AreBitsSet( unsigned char value8, unsigned char flagsToCheck );
bool			 AreBitsSet( unsigned int value32, unsigned int flagsToCheck );
bool			 AreBitsSet(size_t value64, size_t flagsToCheck);
bool			 AreBitsSet(uint16_t value64, uint16_t flagsToCheck);

void			 SetBits   (unsigned char& value8, unsigned char flagsToSet);
void			 SetBits   (unsigned int&  value32, unsigned int flagsToSet);
void			 SetBits   (size_t&  value32, size_t flagsToSet);
void			 SetBits   (uint16_t& value, uint16_t flagToSet);
//void			 SetBits   (uint8_t& value, uint8_t flagToSet);
void			 ClearBits (unsigned char& value8, unsigned char flagToClear);
void			 ClearBits (unsigned int&  value32, unsigned int flagToClear);
void			 ClearBits (size_t&  value32, size_t flagToClear);

void			 RightShift(uint16_t& value, uint16_t count);
void			 RightShift(uint8_t& value,  uint8_t count);
void			 LeftShift (uint16_t& value, uint16_t count);
void			 LeftShift (uint8_t& value,  uint8_t count);

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
Vector3			 Slerp(Vector3 &a, Vector3 &b, float t);
Vector3			 SlerpUnit(Vector3 &a, Vector3 &b, float t);

// EASING FUNCTIONS
float			 SmoothStart2(float t); // 2nd-degree smooth start (a.k.a. �quadratic ease in�)
float			 SmoothStart3(float t); // 3rd-degree smooth start (a.k.a. �cubic ease in�)
float			 SmoothStart4(float t); // 4th-degree smooth start (a.k.a. �quartic ease in�)
float			 SmoothStop2 (float t); // 2nd-degree smooth start (a.k.a. �quadratic ease out�)
float			 SmoothStop3 (float t); // 3rd-degree smooth start (a.k.a. �cubic ease out�)
float			 SmoothStop4 (float t); // 4th-degree smooth start (a.k.a. �quartic ease out�)
float			 SmoothStep3 (float t); // 3rd-degree smooth start/stop (a.k.a. �smoothstep�)
float			 CorssFade	 (float a, float b, float t);

float			 GetShortestDistanceBetweenLineAndPoint(Vector2 position1, Vector2 position2, Vector2 point);

std::string      GetDirectionNameFromType(DIRECTIONS dir);