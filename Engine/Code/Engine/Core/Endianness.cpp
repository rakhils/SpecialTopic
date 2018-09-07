#include "Engine/Core/Endianness.hpp"
#include <iostream>

// CONSTRUCTOR
Endianness::Endianness()
{

}

// DESTRUCTOR
Endianness::~Endianness()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/30
*@purpose : Checks systems Indianess
*@param   : NIL
*@return  : Endianness type of my machine
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eEndianness Endianness::GetPlatformEndianness()
{
	int num = 1;
	char const *array = (char const*)&num;
	if(array[0] == 0x01)
	{
		return LITTLE_ENDIAN;
	}	
	return BIG_ENDIAN;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/30
*@purpose : Convert to data to given endianess
*@param   : size, data and target endianess
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Endianness::ToEndianness(size_t const size, void *data, eEndianness targetEndianness)
{
	if(targetEndianness == GetPlatformEndianness())
	{
		return;
	}

	char *byte_buffer = (char*)data;

	size_t indexI = 0;
	size_t indexJ = size - 1U;
	while(indexI < indexJ)
	{
		std::swap(byte_buffer[indexI], byte_buffer[indexJ]);
		indexI++;
		indexJ--;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Endianness::FromEndianness(size_t const size, void *data, eEndianness SourceEndianness)
{
	if (SourceEndianness != GetPlatformEndianness())
	{
		return;
	}

	char *byte_buffer = (char*)data;

	size_t indexI = 0;
	size_t indexJ = size - 1U;
	while (indexI < indexJ)
	{
		std::swap(byte_buffer[indexI], byte_buffer[indexJ]);
		indexI++;
		indexJ--;
	}
}
