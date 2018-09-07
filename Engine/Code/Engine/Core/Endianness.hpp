#pragma once
#include <stdio.h>
#include <stdint.h>
/*\class  : Endianness		   
* \group  : <GroupName>		   
* \brief  :	   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/30/2018 2:09:31 PM
* \contact: srsrakhil@gmail.com
*/
enum eEndianness
{
	LITTLE_ENDIAN,
	BIG_ENDIAN,
};

class Endianness
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	Endianness();
	~Endianness();

	static eEndianness GetPlatformEndianness();
	static void		   ToEndianness( size_t const size, void *data, eEndianness targetEndianness );
	static void		   FromEndianness( size_t const size, void *data, eEndianness SourceEndianness );

	static uint16_t    ToBigEndian(uint16_t value);

	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};