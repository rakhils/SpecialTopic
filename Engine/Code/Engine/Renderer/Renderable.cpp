#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Mesh/Mesh.hpp"
// CONSTRUCTOR
Renderable::Renderable()
{

}

// DESTRUCTOR
Renderable::~Renderable()
{
	if(m_mesh != nullptr)
	{
		delete m_mesh;
		m_mesh = nullptr;
	}
	delete m_material;
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
void Renderable::SetMesh(Mesh *mesh)
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
void Renderable::SetMaterial(Material *material)
{
	m_material = material;
}
