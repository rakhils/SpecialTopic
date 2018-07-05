#pragma once
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Renderer/Renderer.hpp"

/*\class  : MeshBuilder
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
* \date   : 3/26/2018 4:22:24 PM
*
* \contact: srsrakhil@gmail.com
*/
enum FillMode
{
	FILL_MODE_FILL,
	FILL_MODE_WIRE,
	FILL_MODE_FILL_WIRE
};
class MeshBuilder
{

public:
	//Member_Variables
	VertexMaster m_stamp;
	std::vector<VertexMaster> m_vertices;
	std::vector<int> m_indices;

	DrawInstruction m_draw;
	//Static_Member_Variables

	//Methods

	MeshBuilder();
	~MeshBuilder();
	
	void Begin(DrawPrimitiveType primitives, bool use_indices );
	void End();

	void Reset();
	void SetColor(Rgba rgba);
	void SetUV(Vector2 uv);
	void SetNormal(Vector3 normal);
	void SetTangent(Vector3 tangent);
	void SetBiTangent(Vector3 bitangent);
	int PushVertex(Vector3 position);
	Mesh* AppendMesh(Mesh *mesh);
	Mesh * CreateMesh();
	Mesh *CreateMeshFromVertex_PCUN();
	
	template <typename VERTEX_TYPE>
	Mesh *CreateMeshFromType(VERTEX_TYPE const *buffer,int count)
	{
		Mesh *mesh = new Mesh();
		mesh->SetVertices<VERTEX_TYPE>(static_cast<int>(count), buffer);
		return mesh;
	}
	void SetDrawInstruction(Mesh *mesh);

	void AddQuadIndex(int index1, int index2, int index3, int index4);
	void AddTriangleIndex(int index1, int index2,int index3);
	Vector3 GetNormalFromQuad(Vector3 bottomLeft, Vector3 bottomRigth, Vector3 topLeft, Vector3 topRigth);

	template <typename VERTEX_TYPE>
	Mesh* CreateMesh()
	{
		Mesh *mesh = new Mesh();
		std::vector<VERTEX_TYPE> vertices;
		for (int vertexIndex = 0; vertexIndex < m_vertices.size(); vertexIndex++)
		{
			VERTEX_TYPE vertex = m_vertices.at(vertexIndex);
			vertices.push_back(vertex);
		}


		mesh->SetVertices<VERTEX_TYPE>(static_cast<int>(m_vertices.size()),vertices.data());
		mesh->SetIndices(static_cast<int>(m_indices.size()), m_indices.data());
		mesh->m_drawInstruction = m_draw;
		if (mesh->m_drawInstruction.m_usingIndices)
		{
			mesh->m_drawInstruction.m_elemCount = static_cast<int>(m_indices.size());
		}
		else
		{
			mesh->m_drawInstruction.m_elemCount = static_cast<int>(m_vertices.size());
		}
		return mesh;
	}

	template <typename  VERTEX_TYPE>
	static Mesh* CreateUVSpehere(Vector3 position, float radius, int wedges, int slices, Rgba color, FillMode mode)
	{
		MeshBuilder meshbuilder;
		CreateUVSpehere(meshbuilder, position, radius, wedges, slices, color, mode);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* CreateDistortedUVSpehere(Vector3 position, float radius, int wedges, int slices,float distortlength, Rgba color, FillMode mode)
	{
		MeshBuilder meshbuilder;
		CreateDistortedUVSpehere(meshbuilder, position, radius, wedges, slices,distortlength, color, mode);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* CreateQuad(Vector3 position, Vector3 rigthDirection, Vector3 upDirection, Vector2 dimension, Rgba color, FillMode mode)
	{
		MeshBuilder meshbuilder;
		CreateQuad(meshbuilder, position, rigthDirection, upDirection, dimension, color, mode);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* CreateCube(Vector3 position, Vector3 size, Rgba color,FillMode mode)
	{
		MeshBuilder meshbuilder;
		CreateCube(meshbuilder, position, size, color, mode);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* Create3DPlane(Vector3 position, Vector3 rigthDirection, Vector3 upDirection, Vector2 dimensions, Rgba color,FillMode mode)
	{
		MeshBuilder meshbuilder;
		Create3DPlane(meshbuilder, position, rigthDirection, upDirection, dimensions, color, mode);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* Create3DPlaneChunk(Vector3 position, Vector3 rigthDirection, Vector3 upDirection, Vector2 dimensions, Vector2 chunkSize,Rgba color, FillMode mode)
	{
		MeshBuilder meshbuilder;
		Create3DPlaneChunk(meshbuilder, position, rigthDirection, upDirection, dimensions,chunkSize, color, mode);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* Create3DLine(Vector3 position1, Vector3 position2, Rgba color1, Rgba color2)
	{
		MeshBuilder meshbuilder;
		Create3DLine(meshbuilder, position1, position2 ,color1,color2);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* Create3DPoint(Vector3 position, Rgba color)
	{
		MeshBuilder meshbuilder;
		Create3DPoint(meshbuilder,position,color);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* Create3DWireMesh(Vector3 position, Vector3 size, Rgba color)
	{
		MeshBuilder meshbuilder;
		Create3DWireMesh(meshbuilder,position, size, color);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* Create2DGrid(Vector3 position, Vector2 dimensions,Vector3 rightDirection,Vector3 upDirection, Rgba color)
	{
		MeshBuilder meshbuilder;
		Create2DGrid(meshbuilder, position, dimensions,rightDirection,upDirection,color);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* CreateBasis(Vector3 position, Vector3 rotation,float scale)
	{
		MeshBuilder meshbuilder;
		CreateBasis(meshbuilder, position, rotation,scale);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* CreateLineSet(Vector3 startPosition[],Vector3 endPosition[],int count,Rgba color)
	{
		MeshBuilder meshbuilder;
		CreateLineSet(meshbuilder, startPosition,endPosition,count, color);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	template <typename  VERTEX_TYPE>
	static Mesh* CreateTriangle(Vector3 position1, Vector3 position2,Vector3 position3, Rgba color)
	{
		MeshBuilder meshbuilder;
		meshbuilder.Begin(PRIMITIVE_TRIANGES, true);
		CreateTriangle(meshbuilder, position1, position2, position3, color);
		return meshbuilder.CreateMesh<VERTEX_TYPE>();
	}

	
	static void CreateUVSpehere					 (MeshBuilder &meshbuilder, Vector3 position, float radius, int wedges, int slices, Rgba color, FillMode mode);
	static void CreateDistortedUVSpehere         (MeshBuilder &meshbuilder, Vector3 position, float radius, int wedges, int slices, float distortlength, Rgba color, FillMode mode);
	static void CreateQuad						 (MeshBuilder &meshbuilder, Vector3 position, Vector3 rigthDirection, Vector3 upDirection, Vector2 dimension, Rgba color, FillMode mode);							   
	static void CreateCube						 (MeshBuilder &meshbuilder, Vector3 position, Vector3 size, Rgba color,FillMode FillMode);
	static void Create3DPlane					 (MeshBuilder &meshbuilder, Vector3 position, Vector3 rigthDirection, Vector3 upDirection, Vector2 dimensions, Rgba color,FillMode mode);
	static void Create3DPlaneChunk					 (MeshBuilder &meshbuilder, Vector3 position, Vector3 rigthDirection, Vector3 upDirection, Vector2 dimensions, Vector2 chunkSize,Rgba color,FillMode mode);
	static void Create3DLine					 (MeshBuilder &meshbuilder, Vector3 position1, Vector3 position2, Rgba color1, Rgba color2);
	static void Create3DPoint					 (MeshBuilder &meshbuilder, Vector3 position, Rgba color);
	static void Create3DWireMesh				 (MeshBuilder &meshbuilder, Vector3 position, Vector3 size, Rgba color);
	static void Create2DGrid					 (MeshBuilder &meshbuilder, Vector3 position, Vector2 dimensions,Vector3 rightDirection,Vector3 upDirection, Rgba color);
	static void CreateBasis						 (MeshBuilder &meshbuilder, Vector3 position, Vector3 rotation,float scale);
	static void CreateLineSet					 (MeshBuilder &meshBuilder, Vector3 startPosition[], Vector3 endPosition[], int count,Rgba color);
	static void	CreateTriangle					 (MeshBuilder &meshBuilfer, Vector3 position1, Vector3 position2, Vector3 position3, Rgba color);
	static void	CreateRect					     (MeshBuilder &meshBuilfer, Vector3 position1, Vector3 position2, Vector3 position3,Vector3 position4, Rgba color);
	static Mesh* CreateMeshFromFile				 (std::string imageFilePath);

	static bool GenerateMikkt(MeshBuilder &meshBuilder);
	void GenerateArbitaryTangent(Vector3 *tangent, Vector3 *bitangent, Vector3 normal);
	void RemoveVerticesFromBegin(int count);
	void RemoveIndicesFromBegin(int count);
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