#include "IntVector2.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 IntVector2::operator+(const IntVector2& intVectorA)
{
	return IntVector2(x + intVectorA.x, y + intVectorA.y);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/21
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector2 IntVector2::GetAsVector2()
{
	return Vector2(static_cast<float>(x), static_cast<float>(y));
}

IntVector2 IntVector2::ZERO =  IntVector2(+0, +0);
IntVector2 IntVector2::ONE =   IntVector2(+1, +1);
IntVector2 IntVector2::EAST =  IntVector2(+1, +0);
IntVector2 IntVector2::WEST =  IntVector2(-1, +0);
IntVector2 IntVector2::NORTH = IntVector2(+0, +1);
IntVector2 IntVector2::SOUTH = IntVector2(+0, -1);

IntVector2::IntVector2()
{

}

IntVector2::IntVector2(int x, int y)
{
	this->x = x;
	this->y = y;
}

bool IntVector2::operator==(const Vector2& compare) const
{
	if(x == static_cast<int>(compare.x) && y == static_cast<int>(compare.y))
	{
		return true;
	}
	return false;
}

IntVector2 IntVector2::operator-(const IntVector2& compare)
{
	IntVector2 retValue;
	retValue.x = x - compare.x;
	retValue.y = y - compare.y;
	return retValue;
}

void IntVector2::operator =(Vector2 positionVector)
{
	this->x = static_cast<int>(positionVector.x);
	this->y = static_cast<int>(positionVector.y);
}

bool IntVector2::operator==( const IntVector2& compare ) const
{
	if(x == compare.x && y==compare.y)
	{
		return true;
	}
	return false; 
}

void IntVector2::SetFromText(const char* text)
{
	std::string value;
	value.append(text);
	size_t index = value.find_first_of(',',0);
	std::string xValue = value.substr(0,index); 
	std::string yValue = value.substr(index+1,value.length());
	x = (int)stoi(xValue);
	y = (int)stoi(yValue);
}