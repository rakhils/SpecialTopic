#pragma once
#include "Engine/Core/Endianness.hpp"
/*\class  : BytePacker		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/30/2018 2:44:11 PM
* \contact: srsrakhil@gmail.com
*/
#define BIT_FLAG(f)   (1U << (f))
enum eBytePackerOptionBit : unsigned int
{
	BYTEPACKER_OWNS_MEMORY	= BIT_FLAG(0),
	BYTEPACKER_CAN_GROW		= BIT_FLAG(1)
};
typedef unsigned int eBytePackerOptions;

class BytePacker
{

public:
	//Member_Variables
	eEndianness m_byteOrder;
	size_t		m_currentPosition;
	size_t		m_bufferSize;
	void *		m_buffer;
	//Static_Member_Variables

	//Methods

	BytePacker();
	BytePacker( eEndianness byteOrder = LITTLE_ENDIAN );
	BytePacker( size_t bufferSize, eEndianness byteOrder = LITTLE_ENDIAN );
	BytePacker( size_t bufferSize, void *buffer, eEndianness = LITTLE_ENDIAN );
	~BytePacker();

	void	SetEndianess(eEndianness byteOrder)			{ m_byteOrder = byteOrder; }
	void	SetReadableByteCount(size_t byteCount)		{ m_bufferSize = byteCount;}

	bool	WriteBytes( size_t byteCount, void const *data );// Tries to read into out_data.  Returns how much// ended up being read;
	size_t  ReadBytes ( void *outData, size_t maxByteCount );
	size_t  WriteSize ( size_t size );
	size_t  ReadSize  ( size_t *outSize ); // returns how many bytes read, fills out_size


	
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