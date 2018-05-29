#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"

/*\class  : IndexBuffer
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
* \date   : 3/26/2018 4:54:59 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class IndexBuffer : public RenderBuffer
{

public:
	//Member_Variables
	int m_indexStride;
	int m_indexCount;
	//Static_Member_Variables

	//Methods

	IndexBuffer();
	~IndexBuffer();
	
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