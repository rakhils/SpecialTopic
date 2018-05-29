#pragma once

/*\class  : RenderBuffer
*		   
* \group  : <GroupName>
*		   
* \brief  :
*		   
* \TODO:  :
*		   
* \note   :
*		   
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 1/25/2018 8:31:36 PM
*
* \contact: srsrakhil@gmail.com
*/
 
#include "Engine/Renderer/External/gl/glcorearb.h"
#include "Engine/Renderer/External/gl/glext.h"
#include "Engine/Renderer/External/gl/wglext.h"


class RenderBuffer
{

public:
	//Member_Variables
	GLuint handle = NULL;       // OpenGL handle to the GPU buffer, defualt = NULL;
	size_t buffer_size;  // how many bytes are in this buffer, default = 0
	//Static_Member_Variables

	//Methods

	RenderBuffer();
	~RenderBuffer();

	bool CopyToGPU(size_t const byte_count, void const *data);
	GLuint GetHandle();
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