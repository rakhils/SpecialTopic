#include "Engine/Core/Blob.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
Blob::Blob()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Blob::Blob(Blob &&obj_to_move)
{
	m_buffer = obj_to_move.m_buffer;
	m_buffer_size = obj_to_move.m_buffer_size;

	obj_to_move.m_buffer = nullptr;
	obj_to_move.m_buffer_size = 0;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Blob& Blob::operator=(Blob &&move)
{
	UNUSED(move);
	/*if(this == &move)
	{
		return *this;
	}
	
	free(m_buffer);
	m_buffer_size = 0U;


	m_buffer = obj_to_move.m_buffer;
	m_buffer_size = obj_to_move.m_buffer_size;

	obj_to_move.m_buffer = nullptr;
	obj_to_move.m_buffer_size = 0;
*/


	return *this;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool Blob::set(size_t const byte_count, void const *src)
{
	UNUSED(src);
	UNUSED(byte_count);
	/*resize(byte_count);
	memcpy(m_buffer, src, byte_count);*/
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool Blob::update(size_t const offset, size_t const byte_count, void const *src)
{
	UNUSED(src);
	UNUSED(byte_count);
	UNUSED(offset);
	/*if(offset + byte_count > m_buffer_size)
	{
		return false;
	}
	memcpy((byte_t*)m_buffer + offset, src, byte_count);*/
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool Blob::resize(size_t const new_size)
{
	UNUSED(new_size);
	/*if(m_buffer_size == new_size)
	{
		return true;
	}

	void *buffer = malloc(new_size);
	if(m_buffer != nullptr)
	{
		memcpy(buffer, m_buffer, min(new_size, m_buffer_size));
	}

	free (m_buffer);
	m_buffer*/
	return true;
}

// DESTRUCTOR
Blob::~Blob()
{

}
