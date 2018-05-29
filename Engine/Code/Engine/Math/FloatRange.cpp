#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/MathUtil.hpp"

FloatRange::FloatRange()
{

}

FloatRange::FloatRange(float initialMin, float initialMax)
{
	min = initialMin;
	max = initialMax;
}

FloatRange::FloatRange(float initialMinMax)
{
	initialMinMax = initialMinMax;
}

FloatRange FloatRange::operator+(const FloatRange value)
{
	return FloatRange(value.min+min,value.max+max);
}

float FloatRange::GetRandomInRange() const
{
	return GetRandomFloatInRange(min,max);
}

void FloatRange::SetFromText(const char* text)
{
	std::string value;
	value.append(text);
	size_t index = value.find_first_of('~',0);
	std::string xValue = value.substr(0,index); 
	std::string yValue = value.substr(index+1,value.length());
	min = stof(xValue);
	max = stof(yValue);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/18
*@purpose : Interpolate float range from start to end and return on fractionTowardEnd
*
*@param   : start float range , end float range, fractionTowardsEnd of total of 1
*
*@return  : Interpolated float range at fractionTowardsEnd 
*/
//////////////////////////////////////////////////////////////
FloatRange FloatRange::Interpolate(const FloatRange& start, const FloatRange& end, float fractionTowardEnd)
{
	float finalMin = ::Interpolate(start.min,end.min,fractionTowardEnd);
	float finalMax = ::Interpolate(start.max,end.max,fractionTowardEnd);

	return FloatRange(finalMin,finalMax);
}

