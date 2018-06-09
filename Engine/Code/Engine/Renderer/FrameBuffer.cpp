#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Renderer/GL/glfunctions.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "../Core/ErrorWarningAssert.hpp"
// CONSTRUCTOR
FrameBuffer::FrameBuffer()
{
	glGenFramebuffers( 1, &m_handle ); 
}

// DESTRUCTOR
FrameBuffer::~FrameBuffer()
{
	if (m_handle != NULL) 
	{
		glDeleteFramebuffers(1, &m_handle);
		m_handle = NULL;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : Returns handle
*
*@param   : NIL
*
*@return  : handle
*/
//////////////////////////////////////////////////////////////
GLuint FrameBuffer::GetHandle()
{
	return m_handle;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void FrameBuffer::SetColorTarget(Texture *color_target)
{
	m_color_target = color_target;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void FrameBuffer::SetDepthStencilTarget(Texture *depth_target)
{
	m_depth_stencil_target = depth_target;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : Get dimensions
*
*@param   : NIL
*
*@return  : dimensions
*/
//////////////////////////////////////////////////////////////
IntVector2 FrameBuffer::GetDimensions()
{
	return Texture::GetDefaultColorTargetTexture()->getDimensions();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool FrameBuffer::Finalize()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

	// keep track of which outputs go to which attachments; 
	GLenum targets[1];

	// Bind a color target to an attachment point
	// and keep track of which locations to to which attachments. 
	glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0 + 0,m_color_target->GetHandle(),0);
	// 0 to to attachment 0
	targets[0] = GL_COLOR_ATTACHMENT0 + 0;

	// Update target bindings
	glDrawBuffers(1, targets);
	GLCheckError(__FILE__, __LINE__);
	// Bind depth if available;
	if (m_depth_stencil_target == nullptr) 
	{
		glFramebufferTexture(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,NULL,0);
	}
	else 
	{
		glFramebufferTexture(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,m_depth_stencil_target->GetHandle(),0);
	}

	GLCheckError(__FILE__, __LINE__);

	// Error Check - recommend only doing in debug
#if defined(_DEBUG)
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) 
	{
		//DebuggerPrintf("Failed to create framebuffer:  %u\n\n", status);
		return false;
	}
#endif
	return true;
}
