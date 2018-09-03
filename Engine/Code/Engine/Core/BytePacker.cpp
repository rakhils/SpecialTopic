#include "Engine/Core/BytePacker.hpp"
#include <Windows.h>
#include "Engine/Core/EngineCommon.hpp"

// CONSTRUCTOR
BytePacker::BytePacker()
{

}

BytePacker::BytePacker(eEndianness byte_order /*= LITTLE_ENDIAN */)
{
	m_byteOrder = byte_order;
}

BytePacker::BytePacker(size_t bufferSize, eEndianness byteOrder /*= LITTLE_ENDIAN */)
{
	m_bufferSize = bufferSize;
	m_byteOrder	 = byteOrder;
}

BytePacker::BytePacker(size_t bufferSize, void *buffer, eEndianness byteOrder/*= LITTLE_ENDIAN */)
{
	m_bufferSize = bufferSize;
	m_byteOrder	 = byteOrder;
	m_buffer     = malloc(m_bufferSize);
	memcpy(m_buffer, buffer, bufferSize);
}

// DESTRUCTOR
BytePacker::~BytePacker()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/30
*@purpose : Write bytes to the buffer
*@param   : Byte count and data
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BytePacker::WriteBytes(size_t byteCount, void const *data)
{

	if((m_currentPosition + byteCount)  > m_bufferSize)
	{
		return false;
	}
	//memcpy((byte_t*)m_buffer + offset, src, byte_count); */
	memcpy((char*)m_buffer + m_currentPosition, data, byteCount);
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
	memcpy(outData, m_buffer, maxByteCount);
	return 0;
}
