#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Renderer/Layouts/VertexLayout.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::map< std::string, Mesh*> Mesh::s_meshes;

// CONSTRUCTOR
Mesh::Mesh()
{

}

// DESTRUCTOR
Mesh::~Mesh()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/26
*@purpose : Set indices
*@param   : Count,Indices data
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Mesh::SetIndices(int count, int const *indices)
{
	m_ibo.m_indexCount = count;
	m_ibo.m_indexStride = sizeof(int);
	m_ibo.CopyToGPU(count*m_ibo.m_indexStride, indices);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/26
*@purpose : Sets the draw instruction constructor
*@param   : 
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Mesh::SetDrawInstruction(DrawPrimitiveType type, bool use_indices, int start_index, int elem_count)
{
	m_drawInstruction.m_primitiveType = type;
	m_drawInstruction.m_usingIndices = use_indices;
	m_drawInstruction.m_startIndex = start_index;
	m_drawInstruction.m_elemCount = elem_count;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/26
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Mesh::SetVertices(int vertex_stride, int count, void const *data)
{
	m_vbo.m_vertexCount = count;
	m_vbo.m_vertexStride = vertex_stride;
	m_layout = &VertexMaster::s_LAYOUT; 
	m_vbo.CopyToGPU(count*vertex_stride, data);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/03
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Mesh::SetVertices(int vcount, void const *vertices, VertexLayout const *layout)
{
	m_vbo.CopyToGPU(vcount * layout->m_stride,vertices);
	m_layout = layout;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/03
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
int Mesh::GetVertexStride() const
{
	return m_layout->m_stride;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/04
*@purpose : Creates a mesh from file
*@param   : Mesh file path
*@return  : NIL
*//////////////////////////////////////////////////////////////
Mesh* Mesh::CreateOrGetMesh(const std::string meshFilePath)
{
	if (s_meshes.find(meshFilePath) != s_meshes.end())
	{
		return s_meshes[meshFilePath];
	}
	Mesh *mesh = MeshBuilder::CreateMeshFromFile(meshFilePath);
	s_meshes[meshFilePath] = mesh;
	return mesh;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/10
*@purpose : Creates from file or retrieves mesh from static container
*@param   : file path , should clone or not
*@return  : Mesh pointer
*//////////////////////////////////////////////////////////////
Mesh* Mesh::CreateOrGetMesh(const std::string meshFilePath, bool clone)
{
	if (clone)
	{
		Mesh *mesh = MeshBuilder::CreateMeshFromFile(meshFilePath);
		return mesh;
	}
	return CreateOrGetMesh(meshFilePath);
}
