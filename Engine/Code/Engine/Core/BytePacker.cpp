#include <Windows.h>
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/StringUtils.hpp"
// CONSTRUCTOR

BytePacker::BytePacker(eEndianness byte_order /*= LITTLE_ENDIAN */)
{
	m_bytepackerType = BYTEPACKER_CAN_GROW;
	m_byteOrder = byte_order;
	m_buffer = NULL;
}

BytePacker::BytePacker(size_t bufferSize, eEndianness byteOrder /*= LITTLE_ENDIAN */)
{
	m_bufferSize = bufferSize;
	m_byteOrder	 = byteOrder;
	m_buffer     = (void *)malloc(bufferSize);
}

BytePacker::BytePacker(size_t bufferSize, void *buffer, eEndianness byteOrder/*= LITTLE_ENDIAN */)
{
	m_bufferSize = bufferSize;
	m_byteOrder	 = byteOrder;
	m_buffer     = malloc(m_bufferSize);
	memcpy(m_buffer, buffer, bufferSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BytePacker::operator=(const BytePacker &bytePacker)
{
	m_buffer = malloc(bytePacker.m_bufferSize);
	memcpy(m_buffer, bytePacker.m_buffer, bytePacker.m_bufferSize);

	m_byteOrder				= bytePacker.m_byteOrder;
	m_currentReadPosition   = bytePacker.m_currentReadPosition;
	m_currentWritePosition  = bytePacker.m_currentWritePosition;
	m_bufferSize			= bytePacker.m_bufferSize;
	m_bytepackerType		= bytePacker.m_bytepackerType;
}

// DESTRUCTOR
BytePacker::~BytePacker()
{
	/*if(m_bufferSize > 0 && m_buffer != nullptr)
	{
		delete m_buffer;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/26
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BytePacker::WriteData(size_t size, void const *data)
{
	UNUSED(size);
	UNUSED(data);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/30
*@purpose : Write bytes to the buffer
*@param   : Byte count and data
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BytePacker::WriteBytes(size_t byteCount, void const *data)
{
	switch (m_bytepackerType)
	{
		case BYTEPACKER_OWNS_MEMORY:
		{
			if (m_currentWritePosition + byteCount > m_bufferSize)
			{
				return false;
			}
			//Endianness::ToEndianness(byteCount,(void *)data,m_byteOrder);
			memcpy((char*)m_buffer + m_currentWritePosition, data, byteCount);
			m_currentWritePosition += byteCount;
			break;
		}
		case BYTEPACKER_CAN_GROW:
		{	
			if (byteCount > GetWritableBytesCount())
			{
				//Endianness::ToEndianness(byteCount, (void *)data, m_byteOrder);
				ReallocateWithNewSize(byteCount - GetWritableBytesCount());
				memcpy((char*)m_buffer + m_currentWritePosition, data, byteCount);

				m_currentWritePosition += byteCount;
				std::string str1 = GetBitString();
			}
			else
			{
				memcpy((char*)m_buffer + m_currentWritePosition, data, byteCount);
				m_currentWritePosition += byteCount;
				std::string str1 = GetBitString();
			}
			break;
		}
		default:
			break;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/30
*@purpose : Read bytes from the buffer
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::ReadBytes(void *outData, size_t maxByteCount)
{
	size_t readableSize = GetReadableBytesCount();
	if(readableSize < maxByteCount)
	{
		maxByteCount = readableSize;
	}
	memcpy(outData, (char*)m_buffer + m_currentReadPosition , maxByteCount);
	m_currentReadPosition += maxByteCount;
	return maxByteCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : Encodes and writes 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::WriteSize(size_t size)
{
	if(size == 0)
	{
		return 0;
	}
	size_t tempValue	 = size;
	size_t index		 = 1;
	size_t startLocation = 0;
	size_t endLocation   = 8;
	size_t returnValue   = 0;
	size_t nextByte		 = GetBytes(tempValue, startLocation ,endLocation);

	int byteIndex = 0;
	for(byteIndex = 0;byteIndex < sizeof(size_t);byteIndex++)
	{
		for (int bitIndex = 0; bitIndex < 8; bitIndex++)
		{
			if(bitIndex == 7)
			{
				SetBits(returnValue, index);
				index = index << 1;
				break;
			}
			if (AreBitsSet(tempValue, 1))
			{
				SetBits(returnValue, index);
			}
			tempValue = tempValue >> 1;
			index     = index << 1;
		}
		std::string str = ToBitString(returnValue);
		nextByte = GetBytes(tempValue, startLocation,endLocation);
		
		if (nextByte == 0)
		{
			index = index >> 1;
			ClearBits(returnValue, index);
			WriteBytes(1, (void *)&returnValue);
			//memcpy((char*)m_buffer  + m_currentWritePosition, (void *)&returnValue, 1);
			break;
		}
		WriteBytes(1, (void *)&returnValue);
		//memcpy((char*)m_buffer  + m_currentWritePosition, (void *)&returnValue, 1);
		returnValue = 0;
		index = 1;
	} 
	return byteIndex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : Reads the size
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::ReadSize(size_t *outSize)
{
	std::string str = GetAsString();
	size_t bitIndex = 1;
	size_t bitIndexForRead = 1;
	do
	{
		size_t data = 0;
		ReadBytes((void*)&data, 1);
		
		int index = 0;
		for (index = 0; index < 7; index++)
		{
			if (AreBitsSet(data, bitIndexForRead))
			{
				SetBits(*outSize, bitIndex);
			}
			bitIndex = bitIndex << 1;
			bitIndexForRead = bitIndexForRead << 1;
		}
		if (!AreBitsSet(data, bitIndexForRead))
		{
			break;
		}
		bitIndexForRead = 1;
	} while (true);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/26
*@purpose : read size of 2 bytes
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t BytePacker::ReadSize2()
{
	uint16_t size;
	ReadBytes((void*)&size, 2);
	//Endianness::ToEndianness(2, (void*)&size, BIG_ENDIAN);
	return size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : Writes string to the byte packeter
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BytePacker::WriteString(char const *chararr)
{
	std::string str(chararr);
	switch (m_bytepackerType)
	{
	case BYTEPACKER_OWNS_MEMORY:
		if (m_currentWritePosition + str.length() > m_bufferSize)
		{
			return false;
		}
		WriteSize(str.length() - 1);
		for (int index = 0; index < str.length() - 1; index++)
		{
			memcpy((char*)m_buffer + m_currentWritePosition, (void *)&str.at(index), 1);
			m_currentWritePosition++;
		}
		break;
	case BYTEPACKER_CAN_GROW:
		{
			WriteSize(str.length());
			std::string test1 = GetBitString();
			if (m_currentWritePosition + str.length() > m_bufferSize)
			{
				ReallocateWithNewSize(str.length() - GetWritableBytesCount());
			}
			std::string test2 = GetBitString();
			for (int index = 0; index < str.length(); index++)
			{
				std::string test3  = GetBitString();
				memcpy((char*)m_buffer + m_currentWritePosition, (void *)&str.at(index), 1);
				std::string test4 = GetBitString();
				m_currentWritePosition++;
			}
		}
		break;
	default:
		break;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : Reads the string
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::ReadString(char *out_str, size_t max_byte_size)
{
	if(m_currentReadPosition + max_byte_size < m_currentReadPosition)
	{	
		max_byte_size = m_bufferSize - m_currentReadPosition;
	}

	for (int index = 0; index < max_byte_size; index++)
	{
		memcpy((void *)&out_str[index],(char*)m_buffer + m_currentReadPosition, 1);
		m_currentReadPosition++;
	}
	return max_byte_size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : Writes bool to buffer
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BytePacker::WriteBool(bool value)
{
	char bitValue = 0;
	if (value)
	{
		bitValue = 1;
	}
	WriteBytes(1, (void*)&bitValue);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : Reads bool value
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::ReadBool(bool *out_value)
{
	if(m_currentReadPosition + 1 > m_bufferSize)
	{
		return 0;
	}
	int bitValue = 0;
	memcpy((void *)&bitValue,(char*)m_buffer + m_currentReadPosition, 1);
	m_currentReadPosition++;
	if(bitValue == 0)
	{
		*out_value = false;
	}
	else
	{
		*out_value = true;
	}
	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : Writes color
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BytePacker::WriteColor(Rgba rgba)
{
	WriteBytes(1, (void*)&rgba.r);
	WriteBytes(1, (void*)&rgba.g);
	WriteBytes(1, (void*)&rgba.b);
	WriteBytes(1, (void*)&rgba.a);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : Reads color
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::ReadColor(Rgba *rgba)
{
	if(m_currentReadPosition + 4 <= m_bufferSize)
	{
		ReadBytes((void *)&rgba->r, 1);
		ReadBytes((void *)&rgba->g, 1);
		ReadBytes((void *)&rgba->b, 1);
		ReadBytes((void *)&rgba->a, 1);
		return 4;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BytePacker::ResetWrite()
{
	m_currentWritePosition = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BytePacker::ResetRead()
{
	m_currentReadPosition = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : reset all variables
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BytePacker::Reset()
{
	m_currentReadPosition  = 0;
	m_currentWritePosition = 0;
	m_bufferSize		   = 0;
	delete m_buffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : Get length from data
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BytePacker::GetLengthFromData(uint16_t *size)
{
	memcpy((void *)&size,(char *)m_buffer + m_currentReadPosition, 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : How many bytes written
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::GetWrittenBytesCount() 
{
	return m_currentWritePosition;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::GetWritableBytesCount() 
{
	return m_bufferSize - m_currentWritePosition;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::GetReadableBytesCount() 
{
	return m_bufferSize - m_currentReadPosition;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::GetWriteHead()
{
	return m_currentWritePosition;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::GetReadHead()
{
	return m_currentReadPosition;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : Sets the write head position
*@param   : new position
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BytePacker::SetWriteHead(size_t position)
{
	m_currentWritePosition = position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : Sets read head position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BytePacker::SetReadHead(size_t position)
{
	m_currentReadPosition = position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BytePacker::AdvanceWriteHead(size_t positon)
{
	m_currentWritePosition += positon;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BytePacker::AdvanceReadHead(size_t positon)
{
	m_currentReadPosition += positon;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : Gets bytes from start location to end location
*@param   : Original value,Start,end locations (0 being least significant bit 0th bit)
*@return  : Bites from start to end
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::GetBytes(size_t value, size_t startLocation, size_t endLocation)
{
	size_t one = 1;
	size_t returnValue = 0;
	size_t flag		   = one << startLocation;
	for (size_t index = startLocation; index < endLocation; index++)
	{
		if (AreBitsSet(value, flag))
		{
			SetBits(returnValue, flag);
		}
		flag = flag << 1;
	}

	return returnValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : Reverses all bits in given values
*@param   : Original value
*@return  : Reversed value
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::ReverseBits(size_t value,size_t maxBytes)
{
	size_t size = sizeof(size_t);
	if(maxBytes == 0)
	{
		maxBytes = size;
	}
	std::string str = ToBitString(value);
	size_t bit = 1;
	size_t bitIndexH = bit << (maxBytes * 8 - 1);
	std::string str1 = ToBitString(bitIndexH);
	size_t bitIndexL = 1;
	bool   isLBitSet = false;
	bool   isHBitSet = false;
	size_t reversedData = 0;

	for (int index = 0; index < maxBytes * 8; index++)
	{
		if (AreBitsSet(value, bitIndexL))
		{
			isLBitSet = true;
		}
		if (AreBitsSet(value, bitIndexH))
		{
			isHBitSet = true;
		}
		if (isHBitSet)
		{
			SetBits(reversedData, bitIndexL);
		}
		if (isLBitSet)
		{
			SetBits(reversedData, bitIndexH);
		}

		bitIndexH = bitIndexH >> 1;
		bitIndexL = bitIndexL << 1;
		isLBitSet = false;
		isHBitSet = false;
		if (bitIndexH <= bitIndexL)
		{
			break;
		}
	}
	std::string str12 = ToBitString(reversedData);
	return reversedData;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : Reverses all bits
*@param   : Original value
*@return  : Reversed value
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::ReverseBits(size_t value)
{
	return ReverseBits(value, sizeof(size_t));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BytePacker::ReallocateWithNewSize(size_t extraSize)
{
	char* buffer = nullptr;
	buffer = (char *)malloc(m_bufferSize + extraSize);
	memcpy(buffer, m_buffer, m_bufferSize);
	m_buffer = buffer;
	m_bufferSize = m_bufferSize + extraSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string BytePacker::GetBitString()
{
	size_t size			  = m_bufferSize;
	std::string fullString = "";

	for (int index = 0; index < size; index++)
	{
		char bb = ((char *)m_buffer)[index];
		std::string byteString = ToBitString(bb);
		if(index == size - 1)
		{
			fullString += (byteString);
		}
		else
		{
			fullString += (byteString + " ");
		}
	}
	return (fullString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string BytePacker::GetAsString()
{
	if(m_bufferSize <= 3)
	{
		return "";
	}
	size_t size = m_bufferSize;
	std::string fullString = "";

	for (int index = 3; index < size; index++)
	{
		char bb = ((char *)m_buffer)[index];
		
		fullString += (bb);
	}
	return (fullString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/03
*@purpose : Sets more than 1 bits
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t BytePacker::SetBytes(size_t& value, size_t newvalue,size_t startLocation, size_t endLocation)
{
	size_t	 returnValue	     = 0;
	size_t   one				 = 1;
	size_t   bitIndexNew	     = one << startLocation;
	size_t   bitIndexOriginal	 = 1;

	for (size_t index = startLocation; index < endLocation; index++)
	{
		if (AreBitsSet(newvalue, bitIndexNew))
		{
			SetBits(value, index);
		}
		bitIndexOriginal = bitIndexOriginal << 1;
		bitIndexNew		 = bitIndexNew      << 1;
	}
	return returnValue;
}
