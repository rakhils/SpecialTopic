#include "Vector3.hpp"

#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/Blackboard.hpp"
#include "Engine/Core/StringUtils.hpp"
Vector3 Vector3::ZERO = Vector3(0, 0, 0);
Vector3 Vector3::ONE  = Vector3(1, 1, 1);

Vector3 Vector3::UP			= Vector3(0,1,0);
Vector3 Vector3::RIGHT		= Vector3(1,0,0);
Vector3 Vector3::FORWARD	= Vector3(0,0,1);

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Vector3::GetRandom(Vector3 mins, Vector3 max)
{
	float randomX = GetRandomFloatInRange(mins.x, max.x);
	float randomY = GetRandomFloatInRange(mins.y, max.y);
	float randomZ = GetRandomFloatInRange(mins.z, max.z);
	return Vector3(randomX, randomY, randomZ);
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
Vector3 Vector3::GetRandomValueOnUnitSphere()
{
	float randomX = GetRandomFloatInRange(-1, 1);
	float randomY = GetRandomFloatInRange(-1, 1);
	float randomZ = GetRandomFloatInRange(-1, 1);
	return Vector3(randomX, randomY, randomZ).GetNormalized();
}

Vector3::Vector3(float xval, float yval, float zval)
{
	x = xval;
	y = yval;
	z = zval;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3::Vector3(std::string value)
{
	SetFromText(value.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3::Vector3(Vector2 value)
{
	x = value.x;
	y = value.y;
	z = 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3::Vector3(Vector2 value, float valuez)
{
	x = value.x;
	y = value.y;
	z = valuez;
}

const Vector3 Vector3::operator - ( const Vector3& vecToSub ) const
{
	return Vector3(x-vecToSub.x, y-vecToSub.y,z-vecToSub.z ); // #MP1Fixme
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/24
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Vector3::operator-=(const Vector3& vecToSubtract)
{
	this->x -= vecToSubtract.x;
	this->y -= vecToSubtract.y;
	this->z -= vecToSubtract.z;
}

void Vector3::operator *=( float val) 
{
	x*=val; y*=val ; z*=val;
}

//-----------------------------------------------------------------------------------------------
const Vector3 Vector3::operator *(float uniformScale) 
{
	return Vector3(x*uniformScale, y*uniformScale,z*uniformScale); // #MP1Fixme
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/25
*@purpose : Operator to add 2 Vector3s
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
const Vector3 Vector3::operator+(Vector3 vectora) const
{
	return Vector3(x + vectora.x, y + vectora.y, z + vectora.z);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : += vector
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Vector3::operator+=(const Vector3 vectorA)
{
	this->x += vectorA.x;
	this->y += vectorA.y;
	this->z += vectorA.z;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/24
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool Vector3::operator==(const Vector3 vectorA)
{
	if(x == vectorA.x && y == vectorA.y && z == vectorA.z)
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/24
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool Vector3::operator!=(const Vector3 vectorA)
{
	if (x == vectorA.x && y == vectorA.y && z == vectorA.z)
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : vector 2 to vector 3 conversions
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
const Vector3 Vector3::operator=(const Vector2& vectorb) const
{
	return Vector3(vectorb.x, vectorb.y, 0);
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
const Vector3 Vector3::operator/(float uniformScale) const
{
	return Vector3(x/uniformScale, y/uniformScale, z/uniformScale); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Vector3::operator/=(const float uniformScale)
{
	x/=uniformScale; y/=uniformScale ; z/=uniformScale;
}

const Vector3 operator *(float uniformScale, const Vector3& vecToScale)
{
	return Vector3(vecToScale.x*uniformScale, vecToScale.y*uniformScale,vecToScale.z*uniformScale);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Checks if given vector is greater than the current one
*@param   : Give vector
*@return  : true if less than equal to given vector else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Vector3::operator>=(const Vector3& vectorA) const
{
	if(x >= vectorA.x && y >= vectorA.y && z >= vectorA.z)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Checks if given vector is less the current
*@param   : Given vec3
*@return  : true if less than else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Vector3::operator<=(const Vector3& vectorA) const
{
	if (x <= vectorA.x && y <= vectorA.y && z <= vectorA.z)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Checks if given vector is greater than the current one
*@param   : Give vector
*@return  : true if less than equal to given vector else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Vector3::operator>(const Vector3& vectorA) const
{
	if(x > vectorA.x && y > vectorA.y && z > vectorA.z)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Checks if given vector is less the current
*@param   : Given vec3
*@return  : true if less than else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Vector3::operator<(const Vector3& vectorA) const
{
	if(x < vectorA.x && y < vectorA.y && z < vectorA.z)
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/10
*@purpose : Get length of vector
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
float Vector3::GetLength()
{
	return sqrtf((x*x) + (y*y) + (z*z));
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/10
*@purpose : Ger normalised vector
*
*@param   : NIL
*
*@return  : Normalised vector
*/
//////////////////////////////////////////////////////////////
Vector3 Vector3::GetNormalized()
{
	float length = GetLength();
	Vector3 normalizedVector(x / length, y / length,z/length);
	return normalizedVector;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/17
*@purpose : Converst vector3 to float
*
*@param   : NIL
*
*@return  : vector3 as string
*/
//////////////////////////////////////////////////////////////
std::string Vector3::GetAsString()
{
	std::string vector3String = "";
	vector3String.append(ToString(x)+","+ToString(y)+","+ToString(z));
	return vector3String;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool Vector3::SetFromText(const char *text)
{
	std::string value;
	value.append(text);
	size_t index = value.find_first_of(',', 0);
	size_t zindex = value.find_first_of(',', index + 1);
	std::string xValue = value.substr(0, index);
	std::string yValue = value.substr(index + 1, zindex);
	std::string zValue = value.substr(zindex + 1, value.length());

	float tempx = 0.f;
	float tempy = 0.f;
	float tempz = 0.f;
	if (ToFloat(xValue, &tempx))
	{
		if (ToFloat(yValue, &tempy))
		{
			if(ToFloat(zValue, &tempz))
			{
				x = tempx;
				y = tempy;
				z = tempz;
				return true;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/25
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool Vector3::IsNearZero()
{
	if(GetLength() <= 0.01)
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : returns xy as vector2
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector2 Vector3::GetXY()
{
	return Vector2(x, y);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : Returns XZ as vector2
*
*@param   : NIL
*
*@return  : vector2
*/
//////////////////////////////////////////////////////////////
Vector2 Vector3::GetXZ()
{
	return Vector2(x, z);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : Returns YZ as vector 2
*
*@param   : NIL
*
*@return  : Vector2
*/
//////////////////////////////////////////////////////////////
Vector2 Vector3::GetYZ()
{
	return Vector2(y, z);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/17
*@purpose : retruns xy intvector2
*
*@param   : NIL
*
*@return  : Intvector equvalent
*/
//////////////////////////////////////////////////////////////
IntVector2 Vector3::GetIntVector2XY()
{
	return IntVector2(static_cast<int>(x), static_cast<int>(y));
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/17
*@purpose : returns intvecto2 of xz
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
IntVector2 Vector3::GetIntVector2XZ()
{
	return IntVector2(static_cast<int>(x), static_cast<int>(z));

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/17
*@purpose : returns intvector2 of yz
*
*@param   : NIL
*
*@return  : Intvector2 equivalent
*/
//////////////////////////////////////////////////////////////
IntVector2 Vector3::GetIntVector2YZ()
{
	return IntVector2(static_cast<int>(y), static_cast<int>(z));
}

