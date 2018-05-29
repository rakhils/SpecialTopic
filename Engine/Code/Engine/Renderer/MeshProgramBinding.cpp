#include "Engine/Renderer/MeshProgramBinding.hpp"

// CONSTRUCTOR
MeshProgramBinding::MeshProgramBinding()
{

}

// DESTRUCTOR
MeshProgramBinding::~MeshProgramBinding()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshProgramBinding::SetMesh(Mesh *mesh)
{
	m_mesh = mesh;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshProgramBinding::SetProgram(ShaderProgram *program)
{
	m_program = program;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
GLuint MeshProgramBinding::GetHandle()
{
	return m_vao_id;
}
