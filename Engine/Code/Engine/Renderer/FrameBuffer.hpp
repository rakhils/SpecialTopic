#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/GL/glfunctions.hpp"
/*\class  : FrameBuffer
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
* \date   : 2/12/2018 4:43:31 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class FrameBuffer
{

public:
	//Member_Variables
	GLuint m_handle = NULL;
	Texture *m_color_target;
	Texture *m_depth_stencil_target;
	//Static_Member_Variables

	//Methods

	FrameBuffer();
	~FrameBuffer();
	
	GLuint	    GetHandle();

	void		SetColorTarget(Texture *color_target);
	void		SetDepthStencilTarget(Texture *depth_target);
	IntVector2  GetDimensions();
	bool	    Finalize(); 
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