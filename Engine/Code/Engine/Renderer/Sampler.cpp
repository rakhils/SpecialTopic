#include "Engine/Renderer/Sampler.hpp"

#include "Engine/Renderer/GL/glfunctions.hpp"
Sampler* Sampler::s_defaultSampler = nullptr;
Sampler* Sampler::s_currentSampler = nullptr;
Sampler* Sampler::s_effectSampler  = nullptr;

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
	if (m_sampler_handle == NULL)
	{
		glGenSamplers(1, &m_sampler_handle);
		if (m_sampler_handle == NULL)
		{
			return false;
		}
	}

	// setup wrapping
	glSamplerParameteri(m_sampler_handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_sampler_handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_sampler_handle, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glSamplerParameteri(m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);         // Default: GL_LINEAR
	glSamplerParameteri(m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);					   // Default: GL_LINEAR

	// filtering; 
	//glSamplerParameteri(m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glSamplerParameteri(m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameterf(m_sampler_handle, GL_TEXTURE_MIN_LOD, -10);
	glSamplerParameterf(m_sampler_handle, GL_TEXTURE_MAX_LOD, 10);

	return true;
}

void Sampler::Destroy()
{
	if (m_sampler_handle != NULL) {
		glDeleteSamplers(1, &m_sampler_handle);
		m_sampler_handle = NULL;
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
	return m_sampler_handle;
}
