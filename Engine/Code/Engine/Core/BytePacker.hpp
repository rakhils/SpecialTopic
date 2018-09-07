#pragma once
#include "Engine/Core/Endianness.hpp"
#include <string>
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
enum eBytePackerOptionBit
{
	BYTEPACKER_OWNS_MEMORY,
	BYTEPACKER_CAN_GROW
};
//typedef unsigned int eBytePackerOptions;

class BytePacker
{

public:
	//Member_Variables
	eEndianness m_byteOrder;
	size_t		m_currentReadPosition = 0;
	size_t		m_currentWritePosition= 0;
	size_t		m_bufferSize		  = 0;
	void *		m_buffer;
	eBytePackerOptionBit m_bytepackerType;
	//Static_Member_Variables

	//Methods

	BytePacker( eEndianness byteOrder = LITTLE_ENDIAN );
	BytePacker( size_t bufferSize, eEndianness byteOrder = LITTLE_ENDIAN );
	BytePacker( size_t bufferSize, void *buffer, eEndianness = LITTLE_ENDIAN );
	~BytePacker();


	void		 SetEndianess(eEndianness byteOrder) { m_byteOrder = byteOrder; }
	void		 SetReadableByteCount(size_t byteCount) { m_bufferSize = byteCount; }

	bool		 WriteBytes(size_t byteCount, void const *data);
	size_t		 ReadBytes ( void *outData, size_t maxByteCount );

	size_t		 WriteSize ( size_t size );
	size_t		 ReadSize(size_t *outSize);

	bool		 WriteString( char const *str );
	size_t		 ReadString( char *out_str, size_t max_byte_size );

	bool         WriteBool(bool value);
	size_t		 ReadBool(bool *out_value);
				 
	void		 ResetWrite();
	void		 ResetRead();

	void	     GetLengthFromData(uint16_t *size);

	eEndianness  GetEndianess();

	size_t		 GetWrittenBytesCount();
	size_t	     GetWritableBytesCount(); 
	size_t		 GetReadableBytesCount();

	size_t		 GetWriteHead();
	size_t		 GetReadHead();
	void		 AdvanceWriteHead(size_t positon);
	void		 AdvanceReadHead(size_t positon);

	size_t		 SetBytes(size_t& originalValue, size_t newvalue, size_t startLocation, size_t endLocation);
	size_t		 GetBytes(size_t value, size_t startLocation, size_t endLocation);
	size_t		 ReverseBits(size_t value);
	size_t		 ReverseBits(size_t value,size_t maxBytes);
	void         ReverseBits(size_t size, void *data);

	void         ReallocateWithNewSize(size_t extraSize);
	void		 Peek(size_t length);

	std::string  GetBitString();
	std::string  GetAsString();
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