#pragma once
//#include "Engine/Renderer/External/gl/glext.h"
#include "Engine/Renderer/External/gl/glcorearb.h"
#include "Engine/Renderer/External/gl/glext.h"
#include "Engine/Renderer/External/gl/wglext.h"

/*\class  : MeshProgramBinding
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
* \date   : 4/29/2018 4:49:11 PM
*
* \contact: srsrakhil@gmail.com
*/
class Mesh;
class ShaderProgram;
class MeshProgramBinding
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	MeshProgramBinding();
	~MeshProgramBinding();
	
	void SetMesh(Mesh *mesh);
	void SetProgram(ShaderProgram *program);

	// Get the vao_id
	GLuint GetHandle();
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables
	GLuint			m_vao_id;
	ShaderProgram	*m_program;
	Mesh			*m_mesh;
	//Static_Member_Variables

	//Methods
	void BindMeshToProgram();
	//Static_Methods

};