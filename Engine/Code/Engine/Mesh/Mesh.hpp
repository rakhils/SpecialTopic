#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Math/Transform.hpp"
/*\class  : Mesh
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/26/2018 4:17:23 PM
* \contact: srsrakhil@gmail.com
*/
struct DrawInstruction
{
	DrawPrimitiveType m_primitiveType;
	int m_startIndex;
	int m_elemCount;
	bool m_usingIndices;
};

class VertexLayout;
class MeshBuilder;
class Mesh
{

public:
	//Member_Variables
	VertexBuffer m_vbo;
	IndexBuffer m_ibo;
	
	DrawInstruction		 m_drawInstruction;
	Rgba				 m_currentColor;

	VertexLayout const * m_layout;
	std::vector<Vector3> m_vertices;
	std::vector<Vector2> m_uvs;
	std::vector<Vector3> m_normal;

	//Static_Member_Variables
	static std::map< std::string, Mesh*> s_meshes;
	//Methods

	Mesh();
	~Mesh();

	void SetIndices(int count, int const *indices);
	void SetVertices(int vertex_stride, int count, void const *data);
	void SetDrawInstruction(DrawPrimitiveType type, bool use_indices, int start_index, int elem_count);
	void SetVertices(int count, Vertex_3DPCU const *vertices);
	void SetVertices(int vcount, void const *vertices, VertexLayout const *layout);
	//void FromBuilderForType(MeshBuilder *mb);
	int GetVertexStride() const;

	template <typename VERT_TYPE>
	void FromBuilderForType(MeshBuilder const &mb) // doing this in create mesh for now . todo changes
	{
		int vcount = mb.m_vertices.size();
		VERT_TYPE *vertices = (VERT_TYPE*)malloc(sizeof(VERT_TYPE) * vcount);

		for (int vIndex = 0; vIndex < vcount; vIndex++)
		{
			vertices[vIndex] = VERT_TYPE(mb.m_vertices.at(vIndex));
		}
		SetVertices<VERT_TYPE>(vcount, vertices);
		free(vertices);
	}

	template<typename VERT_TYPE>
	void SetVertices(int count, VERT_TYPE const *vertices)
	{
		m_vbo.CopyToGPU(count * sizeof(VERT_TYPE),vertices);
		m_vbo.m_vertexCount = count;
		m_layout = &VERT_TYPE::s_LAYOUT;
	}
	template<typename VERT_TYPE>
	void SetIndices(int count, VERT_TYPE const *vertices)
	{
		m_ibo.CopyToGPU(count * sizeof(VERT_TYPE), vertices);
		m_ibo.m_vertexCount = count;
		m_layout = &VERT_TYPE::s_LAYOUT;
	}
	//Static_Methods
	Mesh* CreateOrGetMesh(const std::string meshFilePath);
	Mesh* CreateOrGetMesh(const std::string meshFilePath, bool clone);
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