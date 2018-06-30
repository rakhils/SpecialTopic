#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/GL/glfunctions.hpp"
Sampler* Sampler::s_defaultSampler = nullptr;
Sampler* Sampler::s_currentSampler = nullptr;
Sampler* Sampler::s_effectSampler  = nullptr;
Sampler* Sampler::s_shadowSampler = nullptr;

// CONSTRUCTOR
Sampler::Sampler()
{
	
}

// DESTRUCTOR
Sampler::~Sampler()
{
	Destroy();
}

bool Sampler::Create()
{
	// create the sampler handle if needed; 
	if (m_samplerHandle == NULL)
	{
		glGenSamplers(1, &m_samplerHandle);
		if (m_samplerHandle == NULL)
		{
			return false;
		}
	}

	// setup wrapping
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);         // Default: GL_LINEAR
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);					   // Default: GL_LINEAR

	// filtering; 
	//glSamplerParameteri(m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glSamplerParameteri(m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameterf(m_samplerHandle, GL_TEXTURE_MIN_LOD, -1000);
	glSamplerParameterf(m_samplerHandle, GL_TEXTURE_MAX_LOD,  1000);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Sampler::CreateShadow()
{
	// create the sampler handle if needed; 
	if (m_samplerHandle == NULL)
	{
		glGenSamplers(1, &m_samplerHandle);
		if (m_samplerHandle == NULL)
		{
			return false;
		}
	}
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	float color[4];
	Rgba::WHITE.GetAsFloatArray(color);
	glSamplerParameterfv(m_samplerHandle, GL_TEXTURE_BORDER_COLOR, color);

	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// filtering

	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);         // Default: GL_LINEAR
	glSamplerParameteri(m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);         // Default: GL_LINEAR

	//glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	//glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

																					  // level of detail 
	glSamplerParameterf(m_samplerHandle, GL_TEXTURE_MIN_LOD, -1000);
	glSamplerParameterf(m_samplerHandle, GL_TEXTURE_MAX_LOD, 1000);

	return true;
}

void Sampler::Destroy()
{
	if (m_samplerHandle != NULL) {
		glDeleteSamplers(1, &m_samplerHandle);
		m_samplerHandle = NULL;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/03
*@purpose : Retrieve handler
*
*@param   : NIL
*
*@return  : hanlder
*/
//////////////////////////////////////////////////////////////
GLuint Sampler::GetHandle()
{
	return m_samplerHandle;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Sampler* Sampler::CreateShadowSampler()
{
	if(s_shadowSampler == nullptr)
	{
		s_shadowSampler = new Sampler();
		s_shadowSampler->CreateShadow();
	}
	return s_shadowSampler;
}
