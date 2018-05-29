#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/External/gl/glcorearb.h"
#include "Engine/Renderer/External/gl/glext.h"
#include "Engine/Renderer/External/gl/wglext.h"
/*\class  : UniformBuffer	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 4/14/2018 8:58:41 PM
* \contact: srsrakhil@gmail.com
*/
 
class UniformBuffer
{

public:
	//Member_Variables
	//(void*) m_cpuBuffer;
	GLuint m_handle = NULL;     
	size_t  m_size;

	template <typename OBJECT>
	void Set(OBJECT &obj)
	{
		SetCPUBuffer(sizeof(OBJECT), *obj);
	}

	template <typename OBJECT>
	OBJECT * As()
	{
		return (OBJECT*)GetCPUBuffer();
	}
	//Static_Member_Variables

	//Methods

	UniformBuffer();
	~UniformBuffer();
	void SetCPUBuffer(size_t size, const void *data);
	void SetGPUBuffer(size_t size, const void *data);
	void * GetCPUBuffer();
	
	bool UpdateGPU();
	size_t GetSize();
	
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