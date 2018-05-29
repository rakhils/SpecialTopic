#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/GL/glfunctions.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
UniformBuffer::UniformBuffer()
{

}

// DESTRUCTOR
UniformBuffer::~UniformBuffer()
{
	if (m_handle != NULL)
	{
		glDeleteBuffers(1, &m_handle);
		m_handle = NULL;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/17
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void UniformBuffer::SetGPUBuffer(size_t size, const void *data)
{
	if (m_handle == NULL)
	{
		glGenBuffers(1, &m_handle);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_handle);

	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/16
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void * UniformBuffer::GetCPUBuffer()
{
	//return m_cpuBuffer;
	return nullptr;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/16
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool UniformBuffer::UpdateGPU()
{
	if (m_handle == NULL)
	{
		glGenBuffers(1, &m_handle);
	}

	// Bind the buffer to a slot, and copy memory
	// GL_DYNAMIC_DRAW means the memory is likely going to change a lot (we'll get
	// during the second project)
	glBindBuffer(GL_ARRAY_BUFFER, m_handle);
	//glBufferData(GL_ARRAY_BUFFER, m_size, m_cpuBuffer, GL_DYNAMIC_DRAW);

	// buffer_size is a size_t member variable I keep around for 
	// convenience
	return true;
}
