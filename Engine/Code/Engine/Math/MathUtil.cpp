#include <string.h>
#include <math.h>

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/StringUtils.hpp"
float MATH_PIE = (float)3.1415926535897932384626433832795;

float GetRandomFloatInRange(float minInclusive,float maxExclusive)
{
	float randomFloat;
	if(minInclusive < maxExclusive)
	{
		randomFloat = minInclusive + ((float)rand()/((float)RAND_MAX/(maxExclusive-minInclusive)));
	}
	else
	{
		randomFloat = maxExclusive + ((float)rand()/((float)RAND_MAX/(minInclusive-maxExclusive)));
	}
	return randomFloat;
}

float GetRandomFloatZeroToOne()
{
	return ((float)rand()/(float)RAND_MAX);
}

int GetRandomIntInRange(int minInclusive,int maxExclusive)
{
	int randomValue = (rand()%(maxExclusive+1-minInclusive))+ minInclusive;
	return randomValue;
}

int GetRandomIntLessThan(int maxExclusive)
{
	if(maxExclusive == 0)
	{
		return 0;
	}
	return GetRandomIntInRange(0,maxExclusive-1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/13
*@purpose : Calculates random direction
*@param   : NIL
*@return  : random direction
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 GetRandomDirection()
{
	Vector3 direction(GetRandomFloatZeroToOne(), GetRandomFloatZeroToOne(), GetRandomFloatZeroToOne());
	return direction.GetNormalized();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Returns random point from mins to max bounds
*@param   : Mins and max bounds
*@return  : Random point
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 GetRandomPoint(Vector3 minBounds, Vector3 maxBounds)
{
	float x = GetRandomFloatInRange(minBounds.x, maxBounds.x);
	float y = GetRandomFloatInRange(minBounds.y, maxBounds.y);
	float z = GetRandomFloatInRange(minBounds.z, maxBounds.z);
	return Vector3(x, y, z);
}

double GetRandomDoubleInRange(float minInclusive, float maxExclusive)
{
	double random;
	if (minInclusive < maxExclusive)
	{
		random = minInclusive + ((float)rand() / ((float)RAND_MAX / (maxExclusive - minInclusive)));
	}
	else
	{
		random = maxExclusive + ((float)rand() / ((float)RAND_MAX / (minInclusive - maxExclusive)));
	}
	return random;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : Returns a random character from a-z;
*@param   : NIL
*@return  : random char
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char GetRandomCharacter()
{
	int random = GetRandomIntInRange(32, 127);
	return static_cast<char>(random);
}

float GetRadiantFromDegree(float angle)
{
	float radiant = (MATH_PIE/180)*angle;
	return  radiant;
}

float ConvertRadiansToDegrees( float radians )
{
	float degrees = radians/(MATH_PIE/180.0f);
	return degrees;
}

float ConvertDegreesToRadians( float degrees )
{
	float radiant = (MATH_PIE/180.0f)*degrees;
	return  radiant;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/26
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
float ACosDegrees(float value)
{
	return ConvertRadiansToDegrees(acosf(value));
}

float CosDegrees( float degrees )
{
	float ret = cosf(ConvertDegreesToRadians(degrees));
	return ret;
}

float SinDegrees( float degrees )
{
	return sinf(ConvertDegreesToRadians(degrees));
}


Vector2* getDiffOfVectors(Vector2* vector1,Vector2* vector2)
{
	return new Vector2(vector2->x-vector1->x,vector2->y-vector1->y);
}

float getDistanceOfVectors(Vector2 *vector1,Vector2* vector2)
{
	Vector2 *dist = getDiffOfVectors(vector1,vector2);
	return sqrtf((float)(((dist->x)*(dist->x))+((dist->y)*(dist->y))));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Gets max point to contain both vec2s
*@param   : 2 vec2s
*@return  : Max point 
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 GetMax(Vector2 a, Vector2 b)
{
	Vector2 returnValue;
	if (a.x > b.x)
	{
		returnValue.x = a.x;
	}
	else
	{
		returnValue.x = b.x;
	}
	if (a.y > b.y)
	{
		returnValue.y = a.y;
	}
	else
	{
		returnValue.y = b.y;
	}
	return returnValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Gets max point to contain 2 vec3s
*@param   : 2 vec3
*@return  : Max point
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 GetMax(Vector3 a, Vector3 b)
{
	Vector3 returnValue;
	if (a.x > b.x)
	{
		returnValue.x = a.x;
	}
	else
	{
		returnValue.x = b.x;
	}
	if (a.y > b.y)
	{
		returnValue.y = a.y;
	}
	else
	{
		returnValue.y = b.y;
	}
	if (a.z > b.z)
	{
		returnValue.z = a.z;
	}
	else
	{
		returnValue.z = b.z;
	}
	return returnValue;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : Returns max of 2 integers
*@param   : 2 integers
*@return  : max
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GetMax(int a, int b)
{
	if(a > b)
	{
		return a;
	}
	return b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float GetMax(float a, float b)
{
	if(a > b)
	{
		return a;
	}
	return b;
}

float GetMaxOf2(float val1, float val2)
{
	if(val1 > val2)
	{
		return val1;
	}
	return val2;
}

float GetMinOf2(float val1, float val2)
{
	if(val1 < val2)
	{
		return val1;
	}
	return val2;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/23
*@purpose : Check if values in the range of bounds specified
*
*@param   : NIL
*
*@return  : true if is in the range else false
*/
//////////////////////////////////////////////////////////////
bool IsInRangeOf(int value, int lowerLimit, int upperLimit)
{
	if(value >= lowerLimit && value < upperLimit)
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/23
*@purpose : Check values falls in the range of given bounds
*
*@param   : NIL
*
*@return  : true if value in bounds
*/
//////////////////////////////////////////////////////////////
bool isInRangeOf(float value, float lowerLimit, float upperLimit)
{
	if(value >= lowerLimit && value < upperLimit)
	{
		return true;
	}
	return false;
}

Vector2 getDiffOfVectors(const Vector2 a, const Vector2 b)
{
	Vector2 diffVector(b.x-a.x,b.y-a.y);
	return diffVector;
}

float GetDistance( const Vector2 a, const Vector2 b )
{
	Vector2 dist = getDiffOfVectors(a,b);
	return sqrtf((float)(((dist.x)*(dist.x))+((dist.y)*(dist.y))));
}

float GetDistanceSquared( const Vector2 a, const Vector2 b )
{
	Vector2 dist = getDiffOfVectors(a,b);
	return ((float)(((dist.x)*(dist.x))+((dist.y)*(dist.y))));
}

bool DoAABBsOverlap( const AABB2& a, const AABB2& b )
{
	if(a.mins.x > b.maxs.x || b.mins.x > a.maxs.x)
	{
		return false;
	}

	if(a.mins.y > b.maxs.y || b.mins.y > a.maxs.y)
	{
		return false;
	}
	return true;
}

bool DoDiscsOverlap( const Disc2& a, const Disc2& b )
{
	Vector2 centerDistanceVector = a.center - b.center;
	if(centerDistanceVector.GetLength() > (a.radius+b.radius))
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Check if 2 Disc(Center , Radius) Overlaps each other
*
*@param   : Center of 2 Disc and their radii
*
*@return  : true =>  if discs overlaps
*			false => if discs does not overlap
*/
//////////////////////////////////////////////////////////////
bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius)
{
	Vector2 centerDistanceVector = aCenter - bCenter;
	if (centerDistanceVector.GetLength() > (aRadius + bRadius))
	{
		return false;
	}
	return true;
}

bool IsPointInsideAABB2(AABB2 aabb2, Vector2 position)
{
	if(position.x > aabb2.mins.x && position.x < aabb2.maxs.x)
	{
		if(position.y > aabb2.mins.y && position.y < aabb2.maxs.y)
		{
			return true;
		}
	}
	return false;
}

bool DoDiscAndAABBOverlap(const AABB2& aabb2, const Disc2& disk,Vector2 positionVector)
{
	Vector2 distanceVector = Vector2(disk.center.x - aabb2.GetCenter().x, disk.center.y - aabb2.GetCenter().y);
	
	if(positionVector.y == 1 && positionVector.x==0)
	if((disk.center.y + disk.radius >= aabb2.mins.y))// && disk.center.x > aabb2.mins.x && disk.center.x < aabb2.maxs.x)
	{
		return true;
	}

	if(positionVector.y == 0 && positionVector.x==1)
	if((disk.center.x + disk.radius >= aabb2.mins.x))// && disk.center.y > aabb2.mins.y && disk.center.y < aabb2.maxs.y)
	{
		return true;
	}
	
	if(positionVector.y == 0 && positionVector.x==-1)
	if(disk.center.x - disk.radius < aabb2.maxs.x)
	{
		return true;
	}
	if(positionVector.y == -1 && positionVector.x==0)
	if(disk.center.y - disk.radius < aabb2.maxs.y)
	{
		return true;
	}

	Vector2 NE(aabb2.maxs.x,aabb2.maxs.y);
	Vector2 SE(aabb2.maxs.x,aabb2.mins.y);
	Vector2 NW(aabb2.mins.x,aabb2.maxs.y);
	Vector2 SW(aabb2.mins.x,aabb2.mins.y);

	if(Vector2(disk.center - NE).GetLength() < disk.radius)
	{
		return true;
	}
	if(Vector2(disk.center - NW).GetLength() < disk.radius)
	{
		return true;
	}
	if(Vector2(disk.center - SE).GetLength() < disk.radius)
	{
		return true;
	}
	if(Vector2(disk.center - SW).GetLength() < disk.radius)
	{
		return true;
	}

	if(distanceVector.GetLength() < (aabb2.GetDimensions().x/2 + disk.radius))
	{
		return true;
	}
	return false;
}


float GetAbsolute(float value)
{
	if (value < 0)
	{
		value = value * -1;
	}
	return value;
}

int GetAbsolute(int value)
{
	if (value < 0)
	{
		value = value * -1;
	}
	return value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/18
*@purpose : retrives absolute(only positive)
*@param   : Vec2 value
*@return  : Abs value
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 GetAbsolute(Vector2 value)
{
	return Vector2(GetAbsolute(value.x), GetAbsolute(value.y));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/18
*@purpose : retrives absolute(only positive)
*@param   : Vec3 value
*@return  : Abs value
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 GetAbsolute(Vector3 value)
{
	return Vector3(GetAbsolute(value.x), GetAbsolute(value.y), GetAbsolute(value.z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/18
*@purpose : retrives absolute(only positive)
*@param   : Vec3 value
*@return  :Abs value
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector4 GetAbsolute(Vector4 value)
{
	return Vector4(GetAbsolute(value.x), GetAbsolute(value.y), GetAbsolute(value.z), GetAbsolute(value.w));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : Finds the power of a value
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double GetPower(int value, int power)
{
	return pow(value, power);
}

bool IsVectorAlmostEqual(Vector2 value1, Vector2 value2,float diff)
{
	float modulusValue = GetAbsolute(GetDistance(value1,value2));
	if(modulusValue < diff)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsAlmostEqual(Vector3 value1, Vector3 comparion, float diff /*= 0.001*/)
{
	if(IsAlmostEqual(value1.x,comparion.x,diff))
	{
		if (IsAlmostEqual(value1.y, comparion.y, diff))
		{
			if (IsAlmostEqual(value1.z, comparion.z, diff))
			{
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsAlmostEqual(float value1, float comparison, float diff /*= 0.001*/)
{
	if (value1 < comparison)
	{
		if(comparison - value1 < diff)
		{
			return true;
		}
		return false;
	}
	if (value1 > comparison)
	{
		if (value1 - comparison < diff)
		{
			return true;
		}
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Checks if the values is near to 0
*@param   : NIL
*@return  : if near 0 returns true else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsNearZero(float value)
{
	return IsAlmostEqual(value, 0.0f);
}

float GetLargestOf4(float value1, float value2, float value3, float value4)
{
	float greatest = value1;
	if(value2 > greatest)
	{
		greatest = value2;
	}
	if(value3 > greatest)
	{
		greatest = value3;
	}
	if(value4 > greatest)
	{
		greatest = value4;
	}
	return greatest;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/20
*@purpose : Get largest of 8 values mostly directions
*
*@param   : All values in 8 direction
*
*@return  : Larget value
*/
//////////////////////////////////////////////////////////////
float GetLargestOf8(float value1, float value2, float value3, float value4, float value5, float value6, float value7, float value8)
{
	float greatest = value1;
	if (value2 > greatest)
	{
		greatest = value2;
	}
	if (value3 > greatest)
	{
		greatest = value3;
	}
	if (value4 > greatest)
	{
		greatest = value4;
	}
	if (value5 > greatest)
	{
		greatest = value5;
	}
	if (value6 > greatest)
	{
		greatest = value6;
	}
	if (value7 > greatest)
	{
		greatest = value7;
	}
	if (value8 > greatest)
	{
		greatest = value8;
	}
	return greatest;
}

Vector2 GetAABBDiscOverlapDistance(const AABB2& aabb2, const Disc2& disk,Vector2 tileVector,Vector2 positionVector)
{
	Vector2 distanceVector(0,0);

	if(positionVector.x == -1 && positionVector.y == -1)
	{	
		distanceVector.x = aabb2.maxs.x - disk.center.x;
		distanceVector.y = aabb2.maxs.y - disk.center.y;
	}

	if(positionVector.x == 1 && positionVector.y == 1)
	{
		distanceVector.x = aabb2.mins.x - disk.center.x;
		distanceVector.y = aabb2.mins.y - disk.center.y;	
	}

	if(positionVector.x == 1 && positionVector.y == -1)
	{	
		distanceVector.x = aabb2.mins.x - disk.center.x;
		distanceVector.y = aabb2.maxs.y - disk.center.y;
	}

	if(positionVector.x == -1 && positionVector.y == 1)
	{	
		distanceVector.x = aabb2.maxs.x - disk.center.x;
		distanceVector.y = aabb2.mins.y - disk.center.y;
	}
	
	if(positionVector.x == 0 || positionVector.y == 0)
	{
		distanceVector.x = aabb2.GetCenter().x - disk.center.x;
		distanceVector.y = aabb2.GetCenter().y - disk.center.y;
	}


	Vector2 unitDistanceVector = distanceVector.GetNormalized();
	Vector2 radiusVectorTowardsAABB2 = Vector2(GetAbsolute(unitDistanceVector.x)*positionVector.x,GetAbsolute(unitDistanceVector.y)*positionVector.y);
	radiusVectorTowardsAABB2 = radiusVectorTowardsAABB2*disk.radius;
	radiusVectorTowardsAABB2+=disk.center;
	Vector2 tempVector = tileVector + positionVector/2;
	//tempVector = tempVector - unitDistanceVector/2;
	Vector2 overLapDistanceVector = tempVector - radiusVectorTowardsAABB2;
	
	return overLapDistanceVector;		
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : Rangemaps Vector2
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 RangeMap(Vector2 value, Vector2 originalLowerBound, Vector2 originalUpperBound, Vector2 newLowerBound, Vector2 newUpperBound)
{
	float xValue = RangeMapFloat(value.x, originalLowerBound.x, originalUpperBound.x, newLowerBound.x, newUpperBound.x);
	float yValue = RangeMapFloat(value.y, originalLowerBound.y, originalUpperBound.y, newLowerBound.y, newUpperBound.y);
	return Vector2(xValue, yValue);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 RangeMap(Vector3 value, Vector3 originalLowerBound, Vector3 originalUpperBound, Vector3 newLowerBound, Vector3 newUpperBound)
{
	float xValue = RangeMapFloat(value.x, originalLowerBound.x, originalUpperBound.x, newLowerBound.x, newUpperBound.x);
	float yValue = RangeMapFloat(value.y, originalLowerBound.y, originalUpperBound.y, newLowerBound.y, newUpperBound.y);
	float zValue = RangeMapFloat(value.z, originalLowerBound.z, originalUpperBound.z, newLowerBound.z, newUpperBound.z);
	return Vector3(xValue, yValue,zValue);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double RangeMap(double value, double OriginalLowerBound, double OriginalUpperBound, double NewLowerbound, double NewUpperBound)
{
	if (OriginalLowerBound == OriginalUpperBound)
	{
		return NewLowerbound;
	}
	double inRange = OriginalUpperBound - OriginalLowerBound;
	double outRange = NewUpperBound - NewLowerbound;
	double valueRelatedToStart = value - OriginalLowerBound;
	double dividedValue = valueRelatedToStart / inRange;
	double outValueRelatedToStart = dividedValue * outRange;
	double outValue = outValueRelatedToStart + NewLowerbound;
	return outValue;
}

float RangeMapFloat(float value,float OriginalLowerBound,float OriginalUpperBound,float NewLowerbound,float NewUpperBound)
{
	if(OriginalLowerBound == OriginalUpperBound)
	{
		return NewLowerbound;
	}
	float inRange = OriginalUpperBound - OriginalLowerBound;
	float outRange = NewUpperBound - NewLowerbound;
	float valueRelatedToStart = value - OriginalLowerBound;
	float dividedValue = valueRelatedToStart/inRange;
	float outValueRelatedToStart = dividedValue*outRange;
	float outValue = outValueRelatedToStart + NewLowerbound;
	return outValue;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : RangeMap int 
*
*@param   : Value in old bounds,New bounds, old bounds 
*
*@return  : Mapped value
*/
//////////////////////////////////////////////////////////////
float RangeMapInt(int value, int OriginalLowerBound, int OriginalUpperBound, int NewLowerbound, int NewUpperBound)
{
	if (OriginalLowerBound == OriginalUpperBound)
	{
		return static_cast<float>(NewLowerbound);
	}
	float inRange = static_cast<float>(OriginalUpperBound) - static_cast<float>(OriginalLowerBound);
	float outRange = static_cast<float>(NewUpperBound - NewLowerbound);
	float valueRelatedToStart = static_cast<float>(value - OriginalLowerBound);
	float dividedValue = valueRelatedToStart / inRange;
	float outValueRelatedToStart = dividedValue * outRange;
	float outValue = outValueRelatedToStart + NewLowerbound;
	return outValue;
}

float Atan2Degrees(float y,float x)
{
	return ConvertRadiansToDegrees(atan2f(y,x));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : gets the minimum point to contain both vec2s
*@param   : 2 Vector2s
*@return  : Min
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 GetMin(Vector2 a, Vector2 b)
{
	Vector2 returnValue;
	if(a.x < b.x)
	{
		returnValue.x = a.x;
	}
	else
	{
		returnValue.x = b.x;
	}

	if (a.y < b.y)
	{
		returnValue.y = a.y;
	}
	else
	{
		returnValue.y = b.y;
	}
	return returnValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Gets minimum point to contain both vec3s
*@param   : 2 vec3s
*@return  : Min value
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 GetMin(Vector3 a, Vector3 b)
{
	Vector3 returnValue;
	if (a.x < b.x)
	{
		returnValue.x = a.x;
	}
	else
	{
		returnValue.x = b.x;
	}
	if (a.y < b.y)
	{
		returnValue.y = a.y;
	}
	else
	{
		returnValue.y = b.y;
	}
	if (a.z < b.z)
	{
		returnValue.z = a.z;
	}
	else
	{
		returnValue.x = b.z;
	}
	return returnValue;
}

double GetMin(double value1, double value2)
{
	if(value1 < value2)
	{
		return value1;
	}
	return value2;
}

int GetMin(int value1, int value2)
{
	if(value1 < value2)
	{
		return value1;
	}
	return value2;
}

float ClampZeroOrToOne(float value)
{
	if(value > 0.5)
	{
		return 1.0f;
	}
	return 0.0f;
}

float GetFractionInRange(float inValue, float rangeStart, float rangeEnd)
{
	float value = RangeMapFloat(inValue,rangeStart,rangeEnd,0,1);
	return value;
}

bool isBitSet(unsigned char bitflags,unsigned char bittocheck)
{
	return ((bitflags & bittocheck) == bittocheck);
}

//float getNumberInRange
float turnTowards(float start, float end, float maxTurnDegrees)
{
	float angularDisplacement = GetAngularDisplacement(start,end);
	if(angularDisplacement < 0)
	{
		if(start < 0)
		if(180 + start < maxTurnDegrees)
		{
			start = 180;
			maxTurnDegrees = maxTurnDegrees - (180 + start);
		}

		if(start - maxTurnDegrees < end)
		{
			return -maxTurnDegrees;
		}
		return -(start - end);
	}
	if(angularDisplacement > 0)
	{
		if(start > 0)
		if(180 - start < maxTurnDegrees)
		{
			start = -180;
			maxTurnDegrees = maxTurnDegrees - (180 - start);
		}
		if(start + maxTurnDegrees < end)
		{
			return maxTurnDegrees;
		}
		return (end - start);
	}
	return 0;
}

bool IsPointInsideDisc2(Vector2 pointValue, Disc2& disk)
{
	Vector2 distance = pointValue - disk.center;
	if(distance.GetLength() < disk.radius)
	{
		return true;
	}
	return false;
}

float turnTowards1(float *start, float end, float maxDelta)
{
	if(end < 0)
	{
		end = end*-1;
	}
	float returnValue = 0;
	float startCopy = *start;
	if(maxDelta < 0)
	{
		if(*start > end)
		{
			returnValue = maxDelta;
			*start += maxDelta;
	
		}
		if(*start < end)
		{
			*start = end;
			returnValue = end  - startCopy;
		}
	}
	else
	{
		if(*start < end)
		{
			returnValue = maxDelta;
			*start += maxDelta;
		}
		if(*start > end)
		{
			*start = end;
			returnValue = end  - startCopy;
		}
	}
	return returnValue;
}

float TurnTowards2_oroginal(float currentDegrees, float goalDegrees, float maxTurnDegrees)
{
	float diff = GetAngularDisplacement(currentDegrees,goalDegrees);
	float originalCurrentDegrees = currentDegrees;
	float originalGoalDegrees = goalDegrees;
	if(currentDegrees > 0)
	{
		goalDegrees -= currentDegrees;
	}
	else
	{
		goalDegrees += currentDegrees;
	}

	goalDegrees = DoAngleCorrection(goalDegrees);
	currentDegrees = 0;

	if(diff > 0)
	{
		if(currentDegrees + maxTurnDegrees < goalDegrees)
		{
			return originalCurrentDegrees + maxTurnDegrees;
		}
		return originalGoalDegrees;
	}

	if(currentDegrees - maxTurnDegrees > goalDegrees)
	{
		return originalCurrentDegrees - maxTurnDegrees;
	}
	return originalGoalDegrees;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : INCREASES/DECREASES THE ANGLE IN THE DIRECTION OF GOAL DEGREE
*
*@param   : Start angle, Goal angle , Max angle change per frame
*
*@return  : Returns the delta angle of rotation
*/
//////////////////////////////////////////////////////////////
float TurnTowards(float currentDegrees, float goalDegrees, float maxTurnDegrees)
{
	DoAngleCorrection(currentDegrees);
	DoAngleCorrection(goalDegrees);

	//float angularDisplacement = GetAngularDisplacement(currentDegrees,goalDegrees);
	if (currentDegrees > 0)
	{
		goalDegrees -= currentDegrees;
	}
	else
	{
		goalDegrees += currentDegrees;
	}
	//if(angularDisplacement < 0)
	//{	
	//	return TurnTowardsInNegativeDirection(0,goalDegrees,maxTurnDegrees);
	//}
	//else
	{
		return TurnTowardsInPositiveDirection(0,goalDegrees,maxTurnDegrees);
	}
}

float TurnTowardsInNegativeDirection(float angle, float finalAngle, float deltaAngle)
{
	if(angle - deltaAngle > finalAngle)
	{
		/*angle -= deltaAngle;
		return angle;*/
		return deltaAngle;
	}
	angle = finalAngle;
	return angle;
}

float TurnTowardsInPositiveDirection(float m_angle, float m_finalAngle, float deltaAngle)
{
	if(m_angle + deltaAngle < m_finalAngle)
	{
		/*m_angle += deltaAngle;
		return m_angle;*/
		return deltaAngle;
	}
	return m_finalAngle - m_angle;
	/*m_angle = m_finalAngle;
	return m_angle;*/
}

float GetAngularDisplacement(float start,float end)
{
	float angularDisp = end - start;
	while(angularDisp > 180)
	{
		angularDisp -= 360;
	}
	while(angularDisp <-180)
	{
		angularDisp += 360;
	}
	return angularDisp;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/25
*@purpose : Rotates the given direction to original angle + angle given
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector2 Rotate2DVector(Vector2 direction, float angle)
{
	float degrees = Atan2Degrees(direction.y,direction.x);
	degrees += angle;
	return (Vector2(CosDegrees(degrees), SinDegrees(degrees)));
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/18
*@purpose : Rotates vector on y axis
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 RotateOnYAxis(Vector3 direction, float angle)
{	
	Matrix44 yAxisRotation = Matrix44::MakeRotationDegreesInY(angle);
	Matrix44 position(Vector3(direction.x, 0, 0), Vector3(0, direction.y, 0), Vector3(0, 0, direction.z));
	float IValue = DotProduct(direction, Vector3(yAxisRotation.GetIVector()));
	float JValue = DotProduct(direction, Vector3(yAxisRotation.GetJVector()));
	float KValue = DotProduct(direction, Vector3(yAxisRotation.GetKVector()));

	return Vector3(IValue, JValue, KValue);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/18
*@purpose : Rotates on z axis
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 RotateOnZAxis(Vector3 direction, float angle)
{
	Matrix44 yAxisRotation = Matrix44::MakeRotationDegrees2D(angle);
	Matrix44 position(Vector3(direction.x, 0, 0), Vector3(0, direction.y, 0), Vector3(0, 0, direction.z));
	float IValue = DotProduct(direction, Vector3(yAxisRotation.GetIVector()));
	float JValue = DotProduct(direction, Vector3(yAxisRotation.GetJVector()));
	float KValue = DotProduct(direction, Vector3(yAxisRotation.GetKVector()));

	return Vector3(IValue,JValue,KValue);
}

float DoAngleCorrection(float &angle)
{
	while(angle > 180)
	{
		angle -= 360;
	}
	while(angle <-180)
	{
		angle += 360;
	}
	return angle;
}

bool IsAngleWithinRangeOf(float angle, float baseAngle, float range,float direction)
{
	if(direction == 1)
	{
		if(angle > (baseAngle - range))
			return true;
	}

	if(direction == -1)
	{
		if(angle < (baseAngle + range))
			return true;
	}
	return false;
}

float DotProduct(Vector2 a, Vector2 b)
{
	return (a.x*b.x + a.y*b.y);
} 

float DotProduct(Vector3 a, Vector3 b)
{
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}

float DotProduct(Vector4 a, Vector4 b)
{
	return (a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/10
*@purpose : Do cross product
*
*@param   : 2 vector3 values
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 CrossProduct(Vector3 a, Vector3 b)
{
	return Vector3(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

bool CheckRandomChance(float chanceForSuccess)
{
	int randomValue = rand()%100;
	chanceForSuccess = chanceForSuccess*100;
	if(randomValue <= chanceForSuccess)
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/22
*@purpose : Get next higerh mulpt of "multplValue"
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
int GetNextHigerMultiple(int currValue, int multipleValue)
{
	/*CHECK VALIDITY
	int modValue = currValue % multipleValue;
	int nextLowerValue = static_cast<int>(currValue / multipleValue);
	int diff = currValue - nextLowerValue;
	return currValue + (multipleValue - diff);*/
	// MAX VALUE CHECK  = 1000000
	currValue++;
	for(int index = currValue;index < currValue + 1000000;index++)
	{
		if(index == 0)
		{
			//continue;
		}
		if(index % multipleValue == 0)
		{
			return index;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/22
*@purpose : Gives the next lowe value
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
int GetNextLowerMultiple(int currValue, int multipleValue)
{
	currValue--;
	for (int index = currValue; index > currValue - 1000000; index--)
	{
		if (index == 0)
		{
			//continue;
		}
		if (index % multipleValue == 0)
		{
			return index;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float GetFraction(float value)
{
	if(value > 0)
	{
		return value - static_cast<int>(value);
	}
	return (value - static_cast<int>(value))*-1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/02
*@purpose : Loops the vvalue in the interval
*@param   : Value , lower bound , upper bound
*@return  : Returns the value inside interval
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int LoopValue(int value, int lowerBound, int upperBound)
{
	if(value < lowerBound)
	{
		value = upperBound;
	}
	if(value > upperBound)
	{
		value = lowerBound;
	}
	return value;
}

bool HasPercentChance(float value)
{
	float randomValue = GetRandomFloatZeroToOne();
	if (randomValue <= value)
	{
		return true;
	}
	return false;
}

int RoundToNearestInt(float inValue)
{
	int iValue = static_cast<int>(inValue);
	float diffValue = inValue - iValue;
	if(inValue > 0)
	{
		if(diffValue >= 0.5)
		{
			iValue ++;
		}
		return iValue;
	}
	diffValue = iValue - inValue;
	if(diffValue > 0.5)
	{
		iValue --;
	}
	return iValue;

}

int ClampInt(int inValue, int min, int max)
{
	if(inValue < min)
	{
		return min;
	}
	else if(inValue > max)
	{
		return max;
	}
	return inValue;
}

float ClampFloat(float inValue, float min, float max)
{
	if(inValue < min)
	{
		return min;
	}
	else if(inValue > max)
	{
		return max;
	}
	return inValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Clamps double value
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double ClampDouble(double inValue, double min, double max)
{
	if (inValue < min)
	{
		return min;
	}
	else if (inValue > max)
	{
		return max;
	}
	return inValue;
}

float ClampFloatZeroToOne(float inValue)
{
	if(inValue < 0.0f)
	{
		return 0.0f;
	}
	else if(inValue > 1.0f)
	{
		return 1.0f;
	}
	return inValue;
}

float ClampFloatNegativeOneToOne(float inValue)
{
	return ClampFloat(inValue,-1,1);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 ClampVector3(Vector3 inValue, Vector3 minClamp, Vector3 maxClamp)
{
	inValue.x = ClampFloat(inValue.x, minClamp.x, maxClamp.x);
	inValue.y = ClampFloat(inValue.y, minClamp.y, maxClamp.y);
	inValue.z = ClampFloat(inValue.z, minClamp.z, maxClamp.z);
	return inValue;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Solving quadratic equations
*
*@param   : Out vector containing calculated roots, a, b ,c coefficients 
*
*@return  : true if root found
*/
//////////////////////////////////////////////////////////////
bool Quadratic(Vector2 *out, float a, float b, float c)
{
	float discriminant = b * b - 4 * a*c;
	if(discriminant < 0)
	{
		return false;
	}
	out->x = (-b + sqrt(discriminant)) / 2 * a ;
	out->y = (-b - sqrt(discriminant)) / 2 * a;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetRandomSRAND()
{
	srand(static_cast<int>(GetSystemTimeInSeconds()));
}

bool AreBitsSet(unsigned char value, unsigned char flagsToCheck)
{
	if((value & flagsToCheck) == flagsToCheck)
	{
		return true;
	}
	return false;
}

bool AreBitsSet(unsigned int value, unsigned int flagsToCheck)
{
	if((value & flagsToCheck) == flagsToCheck)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : Checks if bits are set
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AreBitsSet(size_t value64, size_t flagsToCheck)
{
	if ((value64 & flagsToCheck) == flagsToCheck)
	{
		return true;
	}
	return false;
}

void SetBits(unsigned char& bitFlags8, unsigned char flagsToSet)
{
	bitFlags8 = (bitFlags8 | flagsToSet);
}

void SetBits(unsigned int& bitFlags32, unsigned int flagsToSet)
{
	bitFlags32 = (bitFlags32 | flagsToSet);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : Sets bit to one at pos
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetBits(size_t& value32, size_t flagsToSet)
{
	value32 = (value32 | flagsToSet);
}

void ClearBits(unsigned char& bitFlags8, unsigned char flagToClear)
{
	bitFlags8 = (bitFlags8 & ~flagToClear);
}

void ClearBits(unsigned int& bitFlags32, unsigned int flagToClear)
{
	bitFlags32 = (bitFlags32 & ~flagToClear);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : Clears bits at position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClearBits(size_t& value32, size_t flagToClear)
{
	value32 = (value32 & ~flagToClear);
}

const Vector2 GetProjectedVector(const Vector2& vectorToProject, const Vector2& projectOnto)
{
	float length = projectOnto.GetLength();
	float dotProductValue = DotProduct(vectorToProject,projectOnto.GetNormalized());
	const Vector2 resultanVector(dotProductValue*projectOnto.x/length,dotProductValue*projectOnto.y/length);

	return resultanVector;
}

const Vector2 GetTransformedIntoBasis(const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ)
{
	float dotProductValueI = DotProduct(originalVector,newBasisI.GetNormalized());
	float dotProductValueJ = DotProduct(originalVector,newBasisJ.GetNormalized());
	Vector2 newProjectedVectorInI = dotProductValueI*newBasisI;
	Vector2 newProjectedVectorInJ = dotProductValueJ*newBasisJ;

	const Vector2 resultanVector(dotProductValueI,dotProductValueJ);
	return resultanVector;
}

const Vector2 GetTransformedOutOfBasis(const Vector2& vectorInBasis, const Vector2& oldBasisI, const Vector2& oldBasisJ)
{
	Vector2 Vx =  oldBasisI * vectorInBasis.x + oldBasisJ * vectorInBasis.y;
	return Vx;
}

void DecomposeVectorIntoBasis(const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ, Vector2& out_vectorAlongI, Vector2& out_vectorAlongJ)
{
	float dotProductValueI = DotProduct(originalVector,newBasisI.GetNormalized());
	float dotProductValueJ = DotProduct(originalVector,newBasisJ.GetNormalized());
	out_vectorAlongI = dotProductValueI*newBasisI;
	out_vectorAlongJ = dotProductValueJ*newBasisJ;
}

const Vector2 Reflect(const Vector2& vectorToProject, const Vector2& projectOnto)
{
	float val = DotProduct(vectorToProject,projectOnto.GetNormalized());
	Vector2 Vn = val*projectOnto;
	return vectorToProject - 2*Vn;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/20
*@purpose : Convert a given point to cartesian points
*
*@param   : Point in Iso axis
*
*@return  : Cartesian point
*/
//////////////////////////////////////////////////////////////
Vector2 ConvertIsoToCartesian(Vector2 isoPoints)
{
	float CartesianX = isoPoints.y + isoPoints.x/2.0f;  
	float CartesianY = isoPoints.y - isoPoints.x/2.0f;

	return Vector2(CartesianX,CartesianY);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/20
*@purpose : Convert Cartesian cordinates to Isometric cordinates
*
*@param   : Cartesian point
*
*@return  : Isometric point
*/
//////////////////////////////////////////////////////////////
Vector2 ConvertCartesianToIso(Vector2 cartesianPoints)
{
	float IsoX = (cartesianPoints.x - cartesianPoints.y)/1.0f;
	float IsoY = (cartesianPoints.x + cartesianPoints.y)/2.0f;
	return Vector2(IsoX,IsoY);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : Converts from polar cord to cartesian cords
*
*@param   : NIL
*
*@return  : X Y in cartesian cords
*/
//////////////////////////////////////////////////////////////
Vector2 ConvertPolarToCartesian(float radius, float angleInDegrees)
{
	return Vector2(radius*CosDegrees(angleInDegrees),radius*SinDegrees(angleInDegrees));
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/22
*@purpose : Converts from cartesian cords to polar
*
*@param   : NIL
*
*@return  : R theta in polar
*/
//////////////////////////////////////////////////////////////
Vector2 ConvertCartesianToPolar(float xPos, float yPos)
{
	float r = sqrtf(xPos*xPos + yPos * yPos);
	float theta = Atan2Degrees(yPos , xPos);
	return Vector2(r, theta);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/20
*@purpose : Converst cartesian to spherical (3D)
*
*@param   : Start position and target (look at) position
*
*@return  : Spherical position
*/
//////////////////////////////////////////////////////////////
Vector3 ConvertCartesianToSpherical(Vector3 position)
{
	float r     = sqrt(position.x*position.x + position.y*position.y + position.z*position.z);
	float theta = Atan2Degrees(position.y,position.x);
//	float phi   = Atan2Degrees(sqrt((position.x*position.x + position.y*position.y)),position.z);
	float phi   = ACosDegrees(position.z/r);
	return Vector3(r,theta,phi);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/20
*@purpose : Converts speherical to cartesian
*
*@param   : NIL
*
*@return  : Cartesian cords
*/
//////////////////////////////////////////////////////////////
Vector3 ConvertSphericalToCartesian(Vector3 position)
{
	float x = position.x * CosDegrees(position.y) * CosDegrees(position.z);
	float y = position.x * SinDegrees(position.z);
	float z = position.x * SinDegrees(position.y) * CosDegrees(position.z);

	//float x = position.x * SinDegrees(position.z) * CosDegrees(position.y);
	//float y = position.x * SinDegrees(position.z) * SinDegrees(position.y);
	//float z = position.x * CosDegrees(position.z);

	//float x = position.x * SinDegrees(position.z) * CosDegrees(position.y);
	//float y = position.x * SinDegrees(position.z) * SinDegrees(position.y);
	//float z = position.x * CosDegrees(position.z);

	return Vector3(x,y,z);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/14
*@purpose : Gets hexa decimal equivalent of number
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string GetHexaDecimalEquivalent(int value)
{
	if(value == 0)
	{
		return "00";
	}
	std::string hex;
	int reminder = 0;
	while(value > 0)
	{
		reminder = value % 16;
		if(reminder > 9)
		{
			char ch = 'A' + static_cast<char>((reminder - 10));
			std::string rem;
			rem += ch;
			hex.insert(0, rem);
			value = value / 16;

			continue;
		}
		hex.insert(0,ToString(reminder));
		value = value / 16;

	}
	return hex;
}

const Vector2 Interpolate(const Vector2& start, const Vector2& end, float fractionTowardEnd)
{
	const Vector2 finalVector = start + (end-start)*(fractionTowardEnd);
	return finalVector;
}

const FloatRange Interpolate(const FloatRange& start, const FloatRange& end, float fractionTowardEnd)
{
	float minInterpolate = Interpolate(start.min,end.min,fractionTowardEnd);
	float maxInterpolate = Interpolate(start.max,end.max,fractionTowardEnd);

	const FloatRange floatRange(minInterpolate,maxInterpolate);
	return floatRange;
}

const AABB2 Interpolate(const AABB2& start, const AABB2& end, float fractionTowardEnd)
{
	Vector2 mins = Interpolate(start.mins,end.mins,fractionTowardEnd);
	Vector2 maxs = Interpolate(start.maxs,end.maxs,fractionTowardEnd);
	AABB2 aabb2(mins,maxs);
	return aabb2;
}

float Interpolate(float start, float end, float fractionTowardEnd)
{
	return start + ((end - start)*(fractionTowardEnd));
}

const Disc2 Interpolate(const Disc2& start, const Disc2& end, float fractionTowardEnd)
{
	Vector2 vectorInterpolate = Interpolate(start.center, end.center, fractionTowardEnd);
	float radiusInterpolate = Interpolate(start.radius, end.radius, fractionTowardEnd);
	const Disc2 disc2(vectorInterpolate, radiusInterpolate);
	return disc2;
}

int Interpolate(int start, int end, float fractionTowardEnd)
{
	return start + RoundToNearestInt((end - start)*(fractionTowardEnd));
}

unsigned char Interpolate(unsigned char start, unsigned char end, float fractionTowardEnd)
{
	return start + static_cast<char>(RoundToNearestInt((end - start)*fractionTowardEnd));
}

const IntVector2 Interpolate(const IntVector2& start, const IntVector2& end, float fractionTowardEnd)
{
	int xInterpolate = Interpolate(start.x, end.x, fractionTowardEnd);
	int yInterpolate = Interpolate(start.y, end.y, fractionTowardEnd);
	IntVector2 intVector(xInterpolate, yInterpolate);
	return intVector;
}

const IntRange Interpolate(const IntRange& start, const IntRange& end, float fractionTowardEnd)
{
	int xInterpolate = Interpolate(start.min, end.min, fractionTowardEnd);
	int yInterpolate = Interpolate(start.max, end.max, fractionTowardEnd);
	IntRange intRange(xInterpolate, yInterpolate);
	return intRange;
}

const Rgba Interpolate(const Rgba& start, const Rgba& end, float fractionTowardEnd)
{
	unsigned char r = Interpolate(start.r, end.r, fractionTowardEnd);
	unsigned char g = Interpolate(start.g, end.g, fractionTowardEnd);
	unsigned char b = Interpolate(start.b, end.b, fractionTowardEnd);
	unsigned char a = Interpolate(start.a, end.a, fractionTowardEnd);

	const Rgba rgba(r, g, b, a);
	return rgba;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
const Vector3 Interpolate(const Vector3& start, const Vector3& end, float fractionTowardEnd)
{
	float x = Interpolate(start.x, end.x, fractionTowardEnd);
	float y = Interpolate(start.y, end.y, fractionTowardEnd);
	float z = Interpolate(start.z, end.z, fractionTowardEnd);

	const Vector3& temp = start + (end - start);
	const Vector3 finalVector(temp.x*fractionTowardEnd,temp.y*fractionTowardEnd,temp.z*fractionTowardEnd);
	return Vector3(x,y,z);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
const Vector4 Interpolate(const Vector4& start, const Vector4& end, float fractionTowardEnd)
{
	const Vector4& temp = start + (end - start);
	const Vector4 finalVector(temp.x*fractionTowardEnd, temp.y*fractionTowardEnd, temp.z*fractionTowardEnd,temp.w*fractionTowardEnd);
	return finalVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Slerp(Vector3 &a, Vector3 &b, float t)
{
	float al	 = a.GetLength();
	float bl	 = b.GetLength();

	float len	 = Interpolate(al, bl, t);
	Vector3 ad = a / al;
	Vector3 bd = b / bl;
	Vector3 u    = SlerpUnit(ad, bd, t);
	return len * u;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 SlerpUnit(Vector3 &a, Vector3 &b, float t)
{
	float			 EPSILON = 0.001f;
	float cosangle = ClampFloat(DotProduct(a, b), -1.0f, 1.0f);
	float angle    = ACosDegrees(cosangle);
	if (angle < EPSILON)
	{
		return Interpolate(a, b, t);
	}
	else 
	{
		float pos_num = SinDegrees(t * angle);
		float neg_num = SinDegrees((1.0f - t) * angle);
		float den	  = SinDegrees(angle);

		return (neg_num / den) * a + (pos_num / den) * b;
	}
}

float SmoothStart2(float t)
{
	return t*t;
}

float SmoothStart3(float t)
{
	return t*t*t;
}

float SmoothStart4(float t)
{
	return t*t*t*t;
}

float SmoothStop2(float t)
{
	return (1 - (1 - t)*(1 - t));
}

float SmoothStop3(float t)
{
	return (1 - (1 - t)*(1 - t)*(1 - t));
}

float SmoothStop4(float t)
{
	return (1 - (1 - t)*(1 - t)*(1 - t)*(1 - t));
}

float SmoothStep3(float t)
{
	return CorssFade(SmoothStart2(t),SmoothStop2(t),t);
}

float CorssFade(float a, float b, float t)
{
	return (a + t*(b-a));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string GetDirectionNameFromType(DIRECTIONS dir)
{
	switch (dir)
	{
	case NORTH_WEST:
		return "NORTH_WEST";
		break;
	case NORTH:
		return "NORTH";
		break;
	case NORTH_EAST:
		return "NORTH_EAST";
		break;
	case EAST:
		return "EAST";
		break;
	case CENTRE:
		return "CENTRE";
		break;
	case WEST:
		return "WEST";
		break;
	case SOUTH_WEST:
		return "SOUTH_WEST";
		break;
	case SOUTH:
		return "SOUTH";
		break;
	case SOUTH_EAST:
		return "SOUTH_EAST";
		break;
	default:
		break;
	}
	return "";
}
