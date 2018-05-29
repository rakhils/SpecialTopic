#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"
/*\class  : VertexBuffer	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/26/2018 4:54:48 PM
* \contact: srsrakhil@gmail.com
*/
class VertexBuffer : public RenderBuffer
{

public:
	//Member_Variables
	int m_vertexCount;
	int m_vertexStride;
	//Static_Member_Variables

	//Methods

	VertexBuffer();
	~VertexBuffer();
	
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