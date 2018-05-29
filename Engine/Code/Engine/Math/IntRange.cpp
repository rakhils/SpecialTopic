#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/MathUtil.hpp"

IntRange::IntRange()
{

}

IntRange::IntRange(int initialMin, int initialMax)
{
	min = initialMin;
	max = initialMax;
}

IntRange::IntRange(const IntRange& copyFrom)
{
	min = copyFrom.min;
	max = copyFrom.max;
}

int IntRange::GetRandomInRange()
{
	return GetRandomIntInRange(min,max);
}

void IntRange::SetFromText(const char* text)
{
	std::string value;
	value.append(text);
	size_t index = value.find_first_of('~',0);
	std::string xValue = value.substr(0,index); 
	std::string yValue = value.substr(index+1,value.length());
	min = stoi(xValue);
	max = stoi(yValue);
}

