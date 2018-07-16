#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/ThirdParty/Math/mikktspace.h"
#include <string>
// CONSTRUCTOR

MeshBuilder::MeshBuilder()
{

}

// DESTRUCTOR
MeshBuilder::~MeshBuilder()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/26
*@purpose : Begins and sets
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::Begin(DrawPrimitiveType primitives, bool use_indices)
{
	m_draw.m_primitiveType = primitives;
	m_draw.m_usingIndices  = use_indices;

	if (use_indices) 
	{
		m_draw.m_startIndex = static_cast<int>(m_indices.size());
	}
	else {
		m_draw.m_startIndex = static_cast<int>(m_vertices.size());
	}
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
void MeshBuilder::End()
{
	int endIndex;
	if (m_draw.m_usingIndices)
	{
		endIndex = static_cast<int>(m_indices.size());
	}
	else 
	{
		endIndex = static_cast<int>(m_vertices.size());
	}

	m_draw.m_elemCount = endIndex - m_draw.m_startIndex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MeshBuilder::Reset()
{
	m_vertices.clear();
	m_indices.clear();
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
void MeshBuilder::SetColor(Rgba rgba)
{
	m_stamp.m_rgba = rgba;
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
void MeshBuilder::SetUV(Vector2 uv)
{
	m_stamp.m_uvs = uv;
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
void MeshBuilder::SetNormal(Vector3 normal)
{
	normal = normal.GetNormalized();
	m_stamp.m_normal = normal;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::SetTangent(Vector3 tangent)
{
	tangent = tangent.GetNormalized();
	m_stamp.m_tangent = tangent;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::SetBiTangent(Vector3 bitangent)
{
	bitangent = bitangent.GetNormalized();
	m_stamp.m_bitangent = bitangent;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/26
*@purpose : Push vertex into vector
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
int MeshBuilder::PushVertex(Vector3 position)
{
	m_stamp.m_position = position;
	m_vertices.push_back(m_stamp);
	return static_cast<int>(m_vertices.size()) - 1;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/09
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Mesh* MeshBuilder::AppendMesh(Mesh *mesh1)
{
	for(int index = 0;index < mesh1->m_vertices.size();index++)
	{
		
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/26
*@purpose : Creates mesh obj and returns
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Mesh * MeshBuilder::CreateMesh()
{
	Mesh *mesh = new Mesh();
	mesh->SetVertices<VertexMaster>(static_cast<int>(m_vertices.size()), m_vertices.data());

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

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/10
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Mesh * MeshBuilder::CreateMeshFromVertex_PCUN()
{
	Mesh *mesh = new Mesh();
	mesh->SetVertices<Vertex_3DPCUN>(static_cast<int>(m_vertices.size()), (Vertex_3DPCUN*)m_vertices.data());
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

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/10
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::SetDrawInstruction(Mesh *mesh)
{
	mesh->m_drawInstruction = m_draw;
	if (mesh->m_drawInstruction.m_usingIndices)
	{
		mesh->m_drawInstruction.m_elemCount = static_cast<int>(m_indices.size());
	}
	else
	{
		mesh->m_drawInstruction.m_elemCount = static_cast<int>(m_vertices.size());
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/26
*@purpose : Adds quad index to the list
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::AddQuadIndex(int index1, int index2, int index3, int index4)
{
	m_indices.push_back(index1);
	m_indices.push_back(index2);
	m_indices.push_back(index3);
	m_indices.push_back(index3);
	m_indices.push_back(index2);
	m_indices.push_back(index4);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/27
*@purpose : Adds index points for a triangle
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::AddTriangleIndex(int index1, int index2, int index3)
{
	m_indices.push_back(index1);
	m_indices.push_back(index2);
	m_indices.push_back(index3);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/03
*@purpose : Finds tangent from given quad positions
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 MeshBuilder::GetNormalFromQuad(Vector3 bottomLeft, Vector3 bottomRigth, Vector3 topLeft, Vector3 topRigth)
{
	UNUSED(topRigth);
	Vector3 tangent   = bottomRigth - bottomLeft;
	Vector3 bitangent = topLeft     - bottomLeft;
	Vector3 normal = CrossProduct(bitangent, tangent);
	normal = normal.GetNormalized();
	return normal;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::CreateUVSpehere(MeshBuilder &meshBuilder,Vector3 position, float radius, int wedges, int slices,Rgba color,FillMode mode)
{
	UNUSED(mode);
	meshBuilder.Begin(PRIMITIVE_TRIANGES, true);

	float PI = 3.14f;
	for (int slice_idx = 0; slice_idx <= slices; slice_idx++)
	{
		float v = (float)slice_idx / ((float)slices - 1.f);
		float azimuth = RangeMapFloat(v, 0, 1, -PI / 2.0f, PI / 2.0f);
		azimuth = ConvertRadiansToDegrees(azimuth);
		for (int wedgeIndex = 0; wedgeIndex <= wedges; ++wedgeIndex)
		{
			float u = (float)wedgeIndex / ((float)wedges - 1.f);
			float rotation = 2.0f * PI * u;

			meshBuilder.SetUV(Vector2(u, v));
			meshBuilder.SetColor(color);
			rotation = ConvertRadiansToDegrees(rotation);

			float x = -1.f * CosDegrees(azimuth) * SinDegrees(rotation) * radius;
			float y = 0.f;
			float z = CosDegrees(azimuth) * CosDegrees(rotation) * radius;

			Vector3 newposition = position + ConvertSphericalToCartesian(Vector3(radius, rotation, azimuth));

			/*Vector4 tangent = Vector4(x, y, z, 1.f);
			tangent.Normalize();*/

			Vector3 tangent = Vector3(x, y, z);
			tangent = tangent.GetNormalized();
			Vector3 normal = (newposition - position);
			normal = normal.GetNormalized();

			meshBuilder.SetNormal(normal);
			meshBuilder.SetTangent(tangent);
			meshBuilder.SetBiTangent(CrossProduct(tangent,normal));

			meshBuilder.PushVertex(newposition);
		}
	}

	for (int slice_idx = 0; slice_idx < slices; ++slice_idx)
	{
		for (int wedge_idx = 0; wedge_idx < wedges; ++wedge_idx)
		{
			/*int bl_idx = wedges * slice_idx + wedge_idx;
			int br_idx = bl_idx + 1;

			int tl_idx = bl_idx + wedges;
			int tr_idx = tl_idx + 1;*/
			//y * (wedges + 1U);
			/*int bl_idx = slice_idx * (wedges + 1);//wedges * slice_idx + wedge_idx;
			int br_idx = bl_idx + 1;

			int tl_idx = (slice_idx + 1)*(wedges + 1);//   bl_idx + wedges;
			int tr_idx = tl_idx + 1;*/


			int bl_idx = wedges * slice_idx + wedge_idx;
			int br_idx = bl_idx + 1;

			int tl_idx = bl_idx + wedges;
			int tr_idx = tl_idx + 1;

			meshBuilder.AddQuadIndex(bl_idx, br_idx, tl_idx, tr_idx);
		}
	}
	meshBuilder.End();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::CreateDistortedUVSpehere(MeshBuilder &meshBuilder, Vector3 position, float radius, int wedges, int slices, float distortlength,Rgba color, FillMode mode)
{
	UNUSED(mode);
	meshBuilder.Begin(PRIMITIVE_TRIANGES, true);

	float PI = 3.14f;
	for (int slice_idx = 0; slice_idx <= slices; slice_idx++)
	{
		float v = (float)slice_idx / ((float)slices - 1.f);
		float azimuth = RangeMapFloat(v, 0, 1, -PI / 2.0f, PI / 2.0f);
		azimuth = ConvertRadiansToDegrees(azimuth);
		for (int wedgeIndex = 0; wedgeIndex <= wedges; ++wedgeIndex)
		{
			float u = (float)wedgeIndex / ((float)wedges - 1.f);
			float rotation = 2.0f * PI * u;

			meshBuilder.SetUV(Vector2(u, v));
			meshBuilder.SetColor(color);
			rotation = ConvertRadiansToDegrees(rotation);

			float x = -1.f * CosDegrees(azimuth) * SinDegrees(rotation) * radius;
			float y = 0.f;
			float z = CosDegrees(azimuth) * CosDegrees(rotation) * radius;

			Vector3 newposition = position + ConvertSphericalToCartesian(Vector3(radius, rotation, azimuth));
			Vector3 normal = (newposition - position);
			float randomLength = GetRandomFloatInRange(-distortlength, distortlength);
			newposition = newposition + normal.GetNormalized() * randomLength;
			/*Vector4 tangent = Vector4(x, y, z, 1.f);
			tangent.Normalize();*/

			Vector3 tangent = Vector3(x, y, z);
			tangent = tangent.GetNormalized();
			//normal = (newposition - position);
			normal = normal.GetNormalized();

			meshBuilder.SetNormal(normal);
			meshBuilder.SetTangent(tangent);
			meshBuilder.SetBiTangent(CrossProduct(tangent,normal));

			meshBuilder.PushVertex(newposition);
		}
	}

	for (int slice_idx = 0; slice_idx < slices; ++slice_idx)
	{
		for (int wedge_idx = 0; wedge_idx < wedges; ++wedge_idx)
		{
			/*int bl_idx = wedges * slice_idx + wedge_idx;
			int br_idx = bl_idx + 1;

			int tl_idx = bl_idx + wedges;
			int tr_idx = tl_idx + 1;*/

			int bl_idx = wedges * slice_idx + wedge_idx;
			int br_idx = bl_idx + 1;

			int tl_idx = bl_idx + wedges;
			int tr_idx = tl_idx + 1;

			meshBuilder.AddQuadIndex(bl_idx, br_idx, tl_idx, tr_idx);
		}
	}
	meshBuilder.End();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/31
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::CreateQuad(MeshBuilder &meshBuilder,Vector3 position, Vector3 rigthDirection, Vector3 upDirection, Vector2 dimension, Rgba color, FillMode mode)
{
	UNUSED(mode);
	Vector3 BottomLeft  = position - rigthDirection * dimension.x - upDirection * dimension.y;
	Vector3 BottomRigth = position + rigthDirection * dimension.x - upDirection * dimension.y;
	Vector3 topLeft     = position - rigthDirection * dimension.x + upDirection * dimension.y;
	Vector3 topRight    = position + rigthDirection * dimension.x + upDirection * dimension.y;

	meshBuilder.Begin(PRIMITIVE_TRIANGES, true);
	int index = static_cast<int>(meshBuilder.m_indices.size());
	Vector3 normal = meshBuilder.GetNormalFromQuad(BottomLeft, BottomRigth, topLeft, topRight);
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetColor(color);
	meshBuilder.SetNormal(normal);
	meshBuilder.SetTangent(BottomRigth - BottomLeft);
	meshBuilder.SetBiTangent(topLeft - BottomLeft);
	meshBuilder.PushVertex(BottomLeft);

	meshBuilder.SetUV(Vector2(1, 0));
	meshBuilder.SetColor(color);
	meshBuilder.SetNormal(normal);
	meshBuilder.SetTangent(BottomRigth - BottomLeft);
	meshBuilder.SetBiTangent(topLeft - BottomLeft);
	meshBuilder.PushVertex(BottomRigth);

	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(BottomRigth - BottomLeft);
	meshBuilder.SetBiTangent(topLeft - BottomLeft);
	meshBuilder.PushVertex(topLeft);

	meshBuilder.SetUV(Vector2(1, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(BottomRigth - BottomLeft);
	meshBuilder.SetBiTangent(topLeft - BottomLeft);
	meshBuilder.PushVertex(topRight);

	meshBuilder.AddQuadIndex(index, index + 1, index + 2, index + 3);

	meshBuilder.End();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/27
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::CreateCube(MeshBuilder &meshBuilder,Vector3 position, Vector3 size, Rgba color,FillMode mode)
{
	UNUSED(mode);

	meshBuilder.Begin(PRIMITIVE_TRIANGES, true);
	Vector3 frontFaceCentre = position - Vector3(0, 0, size.z);
	Vector3 backFaceCentre  = position + Vector3(0, 0, size.z);

	Vector3 frontBottomLeftCorner  = frontFaceCentre  + Vector3(-size.x,-size.y, 0);
	Vector3 frontBottomRigthCorner = frontFaceCentre  + Vector3( size.x,-size.y, 0);
	Vector3 frontTopLeftCorner     = frontFaceCentre  + Vector3(-size.x, size.y, 0);
	Vector3 frontTopRigthCorner    = frontFaceCentre  + Vector3( size.x, size.y, 0);

	Vector3 backBottomLeftCorner  = backFaceCentre + Vector3(-size.x,-size.y, 0);
	Vector3 backBottomRigthCorner = backFaceCentre + Vector3( size.x,-size.y, 0);
	Vector3 backTopLeftCorner     = backFaceCentre + Vector3(-size.x, size.y, 0);
	Vector3 backTopRigthCorner    = backFaceCentre + Vector3( size.x, size.y, 0);
	int index = 0;
	
	Vector3 normal = meshBuilder.GetNormalFromQuad(frontBottomLeftCorner, frontBottomRigthCorner, frontTopLeftCorner, frontTopRigthCorner);
	// front
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontBottomRigthCorner - frontBottomLeftCorner);
	meshBuilder.SetBiTangent(frontTopLeftCorner - frontBottomLeftCorner);
	meshBuilder.PushVertex(frontBottomLeftCorner);

	meshBuilder.SetUV(Vector2(1, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontBottomRigthCorner - frontBottomLeftCorner);
	meshBuilder.SetBiTangent(frontTopLeftCorner - frontBottomLeftCorner);
	meshBuilder.PushVertex(frontBottomRigthCorner);

	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontBottomRigthCorner - frontBottomLeftCorner);
	meshBuilder.SetBiTangent(frontTopLeftCorner - frontBottomLeftCorner);
	meshBuilder.PushVertex(frontTopLeftCorner);

	meshBuilder.SetUV(Vector2(1, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontBottomRigthCorner - frontBottomLeftCorner);
	meshBuilder.SetBiTangent(frontTopLeftCorner - frontBottomLeftCorner);
	meshBuilder.PushVertex(frontTopRigthCorner);

	meshBuilder.AddQuadIndex(index, index + 1,index + 2,index + 3);
	index += 4;
	//////////////////////////////////////////////////////////////////////////
	//back
	normal = meshBuilder.GetNormalFromQuad(backBottomRigthCorner, backBottomLeftCorner, backTopLeftCorner, backTopRigthCorner);
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomLeftCorner - backBottomRigthCorner);
	meshBuilder.SetBiTangent(backTopRigthCorner - backBottomRigthCorner);
	meshBuilder.PushVertex(backBottomRigthCorner);
	
	meshBuilder.SetUV(Vector2(1, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomLeftCorner - backBottomRigthCorner);
	meshBuilder.SetBiTangent(backTopRigthCorner - backBottomRigthCorner);
	meshBuilder.PushVertex(backBottomLeftCorner);
	
	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomLeftCorner - backBottomRigthCorner);
	meshBuilder.SetBiTangent(backTopRigthCorner - backBottomRigthCorner);
	meshBuilder.PushVertex(backTopRigthCorner);
	
	meshBuilder.SetUV(Vector2(1, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomLeftCorner - backBottomRigthCorner);
	meshBuilder.SetBiTangent(backTopRigthCorner - backBottomRigthCorner);
	meshBuilder.PushVertex(backTopLeftCorner);
	meshBuilder.AddQuadIndex(index, index + 1, index + 2, index + 3);
	index += 4;
	//////////////////////////////////////////////////////////////////////////
	//top face
	normal = meshBuilder.GetNormalFromQuad(frontTopLeftCorner, frontTopRigthCorner, backTopLeftCorner, backTopRigthCorner);
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontTopRigthCorner - frontTopLeftCorner);
	meshBuilder.SetBiTangent(backTopLeftCorner - frontTopLeftCorner);
	meshBuilder.PushVertex(frontTopLeftCorner);
	
	meshBuilder.SetUV(Vector2(1, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontTopRigthCorner - frontTopLeftCorner);
	meshBuilder.SetBiTangent(backTopLeftCorner - frontTopLeftCorner);
	meshBuilder.PushVertex(frontTopRigthCorner);
	
	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontTopRigthCorner - frontTopLeftCorner);
	meshBuilder.SetBiTangent(backTopLeftCorner - frontTopLeftCorner);
	meshBuilder.PushVertex(backTopLeftCorner);
	
	meshBuilder.SetUV(Vector2(1, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontTopRigthCorner - frontTopLeftCorner);
	meshBuilder.SetBiTangent(backTopLeftCorner - frontTopLeftCorner);
	meshBuilder.PushVertex(backTopRigthCorner);
	meshBuilder.AddQuadIndex(index, index + 1, index + 2, index + 3);
	index += 4;
	//////////////////////////////////////////////////////////////////////////
	//bottom face
	normal = meshBuilder.GetNormalFromQuad(backBottomLeftCorner, backBottomRigthCorner, frontBottomLeftCorner, frontBottomRigthCorner);
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomRigthCorner - backBottomLeftCorner);
	meshBuilder.SetBiTangent(frontBottomLeftCorner - backBottomLeftCorner);
	meshBuilder.PushVertex(backBottomLeftCorner);
	
	meshBuilder.SetUV(Vector2(1, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomRigthCorner - backBottomLeftCorner);
	meshBuilder.SetBiTangent(frontBottomLeftCorner - backBottomLeftCorner);
	meshBuilder.PushVertex(backBottomRigthCorner);
	
	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomRigthCorner - backBottomLeftCorner);
	meshBuilder.SetBiTangent(frontBottomLeftCorner - backBottomLeftCorner);
	meshBuilder.PushVertex(frontBottomLeftCorner);
	
	meshBuilder.SetUV(Vector2(1, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomRigthCorner - backBottomLeftCorner);
	meshBuilder.SetBiTangent(frontBottomLeftCorner - backBottomLeftCorner);
	meshBuilder.PushVertex(frontBottomRigthCorner);
	meshBuilder.AddQuadIndex(index, index + 1, index + 2, index + 3);
	index += 4;
	//////////////////////////////////////////////////////////////////////////
	// LEFT SIDE FACE
	normal = meshBuilder.GetNormalFromQuad(backBottomLeftCorner, frontBottomLeftCorner, backTopLeftCorner, frontTopLeftCorner);
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontBottomLeftCorner - backBottomLeftCorner);
	meshBuilder.SetBiTangent(backTopLeftCorner - backBottomLeftCorner);
	meshBuilder.PushVertex(backBottomLeftCorner);
	
	meshBuilder.SetUV(Vector2(1, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontBottomLeftCorner - backBottomLeftCorner);
	meshBuilder.SetBiTangent(backTopLeftCorner - backBottomLeftCorner);
	meshBuilder.PushVertex(frontBottomLeftCorner);
	
	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontBottomLeftCorner - backBottomLeftCorner);
	meshBuilder.SetBiTangent(backTopLeftCorner - backBottomLeftCorner);
	meshBuilder.PushVertex(backTopLeftCorner);
	
	meshBuilder.SetUV(Vector2(1, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(frontBottomLeftCorner - backBottomLeftCorner);
	meshBuilder.SetBiTangent(backTopLeftCorner - backBottomLeftCorner);
	meshBuilder.PushVertex(frontTopLeftCorner);
	meshBuilder.AddQuadIndex(index, index + 1, index + 2, index + 3);
	index += 4;
	//////////////////////////////////////////////////////////////////////////
	// RIGTH SIDE
	normal = meshBuilder.GetNormalFromQuad(frontBottomRigthCorner, backBottomRigthCorner, frontTopRigthCorner, backTopRigthCorner);
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomRigthCorner - frontBottomRigthCorner);
	meshBuilder.SetBiTangent(frontTopRigthCorner - frontBottomRigthCorner);
	meshBuilder.PushVertex(frontBottomRigthCorner);
	
	meshBuilder.SetUV(Vector2(1, 0));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomRigthCorner - frontBottomRigthCorner);
	meshBuilder.SetBiTangent(frontTopRigthCorner - frontBottomRigthCorner);
	meshBuilder.PushVertex(backBottomRigthCorner);
	
	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomRigthCorner - frontBottomRigthCorner);
	meshBuilder.SetBiTangent(frontTopRigthCorner - frontBottomRigthCorner);
	meshBuilder.PushVertex(frontTopRigthCorner);
	
	meshBuilder.SetUV(Vector2(1, 1));
	meshBuilder.SetNormal(normal);
	meshBuilder.SetColor(color);
	meshBuilder.SetTangent(backBottomRigthCorner - frontBottomRigthCorner);
	meshBuilder.SetBiTangent(frontTopRigthCorner - frontBottomRigthCorner);
	meshBuilder.PushVertex(backTopRigthCorner);
	meshBuilder.AddQuadIndex(index, index + 1, index + 2, index + 3);
	index += 4;
	//////////////////////////////////////////////////////////////////////////

	meshBuilder.End();
}


//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/27
*@purpose : Creates a plane mesh and returns
*@param   : Position of mesh. mesh rigth vector in world,mesh up vector in world, 2diementions color
*@return  : returns mesh obj
*//////////////////////////////////////////////////////////////
void MeshBuilder::Create3DPlane(MeshBuilder &meshBuilder,Vector3 position, Vector3 rigthDirection, Vector3 upDirection, Vector2 dimensions, Rgba color,FillMode mode)
{
	UNUSED(mode);
	meshBuilder.Begin(PRIMITIVE_TRIANGES, true);
	int index = static_cast<int>(meshBuilder.m_vertices.size());
	Vector3 BottomLeftCorner  = position - rigthDirection * dimensions.x - upDirection * dimensions.y;
	Vector3 BottomRigthCorner = position + rigthDirection * dimensions.x - upDirection * dimensions.y;
	Vector3 TopLeftCorner	  = position - rigthDirection * dimensions.x + upDirection * dimensions.y;
	Vector3 TopRigthCorner    = position + rigthDirection * dimensions.x + upDirection * dimensions.y;
	Vector3 normal = meshBuilder.GetNormalFromQuad(BottomLeftCorner, BottomRigthCorner, TopLeftCorner, TopRigthCorner);

	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetColor(color);
	meshBuilder.SetNormal(normal);
	meshBuilder.SetTangent(BottomRigthCorner - BottomLeftCorner);
	meshBuilder.SetBiTangent(TopLeftCorner - BottomLeftCorner);
	meshBuilder.PushVertex(BottomLeftCorner);

	meshBuilder.SetUV(Vector2(1, 0));
	meshBuilder.SetColor(color);
	meshBuilder.SetNormal(normal);
	meshBuilder.SetTangent(BottomRigthCorner - BottomLeftCorner);
	meshBuilder.SetBiTangent(TopLeftCorner - BottomLeftCorner);
	meshBuilder.PushVertex(BottomRigthCorner);

	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetColor(color);
	meshBuilder.SetNormal(normal);
	meshBuilder.SetTangent(BottomRigthCorner - BottomLeftCorner);
	meshBuilder.SetBiTangent(TopLeftCorner - BottomLeftCorner);
	meshBuilder.PushVertex(TopLeftCorner);

	meshBuilder.SetUV(Vector2(1, 1));
	meshBuilder.SetColor(color);
	meshBuilder.SetNormal(normal);
	meshBuilder.SetTangent(BottomRigthCorner - BottomLeftCorner);
	meshBuilder.SetBiTangent(TopLeftCorner - BottomLeftCorner);
	meshBuilder.PushVertex(TopRigthCorner);
	meshBuilder.AddQuadIndex(index, index + 1, index + 2, index + 3);

	meshBuilder.End();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/25
*@purpose : Create plane with chunks of UVs
*@param   : Position of mesh. mesh rigth vector in world,mesh up vector in world, 2diementions color
*@return  : returns mesh obj
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MeshBuilder::Create3DPlaneChunk(MeshBuilder &meshbuilder, Vector3 position, Vector3 rigthDirection, Vector3 upDirection, Vector2 dimensions, Vector2 chunkSize, Rgba color, FillMode mode)
{
	Vector3 copyPosition = position;
	for(int indexX = 0;indexX < static_cast<int>(dimensions.y/chunkSize.y);indexX++)
	{
		for(int indexY = 0;indexY < static_cast<int>(dimensions.x/chunkSize.x);indexY++)
		{
			Create3DPlane(meshbuilder, copyPosition, rigthDirection, upDirection, chunkSize,color, mode);
			copyPosition.x += (chunkSize.x*2);
		}
		copyPosition = position;
		copyPosition.z += chunkSize.y*2*(indexX+1);

	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Creates a line mesh with start color and end color
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::Create3DLine(MeshBuilder &meshBuilder,Vector3 position1, Vector3 position2, Rgba color1, Rgba color2)
{
	meshBuilder.Begin(PRIMITIVE_LINES, true);
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetColor(color1);
	meshBuilder.PushVertex(position1);

	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetColor(color2);
	meshBuilder.PushVertex(position2);
	meshBuilder.m_indices.push_back(0);
	meshBuilder.m_indices.push_back(1);
	//meshBuilder.AddTriangleIndex(index, index + 1, index + 2);

	meshBuilder.End();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::Create3DPoint(MeshBuilder &meshBuilder,Vector3 position, Rgba color)
{
	meshBuilder.Begin(PRIMITIVE_POINTS, false);
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetNormal(Vector3(1, 1, 1));
	meshBuilder.SetColor(color);
	meshBuilder.PushVertex(position);

	meshBuilder.m_indices.push_back(0);
	meshBuilder.End();
	meshBuilder.m_draw.m_elemCount = 1;

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Draws wire mesh cube
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::Create3DWireMesh(MeshBuilder &meshBuilder,Vector3 position, Vector3 size, Rgba color)
{
	meshBuilder.Begin(PRIMITIVE_LINES, true);
	float wiredistance = 0.25f;
	Vector3 frontFaceCentre = position - Vector3(0, 0, size.z);
	Vector3 backFaceCentre = position + Vector3(0, 0, size.z);

	Vector3 frontBottomLeftCorner  = frontFaceCentre + Vector3(-size.x, -size.y, 0);
	Vector3 frontBottomRigthCorner = frontFaceCentre + Vector3(size.x, -size.y, 0);
	Vector3 frontTopLeftCorner	   = frontFaceCentre + Vector3(-size.x, size.y, 0);
	Vector3 frontTopRigthCorner    = frontFaceCentre + Vector3(size.x, size.y, 0);

	Vector3 backBottomLeftCorner  = backFaceCentre + Vector3(-size.x, -size.y, 0);
	Vector3 backBottomRigthCorner = backFaceCentre + Vector3(size.x, -size.y, 0);
	Vector3 backTopLeftCorner     = backFaceCentre + Vector3(-size.x, size.y, 0);
	Vector3 backTopRigthCorner    = backFaceCentre + Vector3(size.x, size.y, 0);

	Vector3 startPosition = frontBottomLeftCorner;
	Vector3 endPosition = frontBottomRigthCorner;
	int index = 0;
	for(float z = -size.z;z< size.z;z+= wiredistance)
	{
		for(float y = -size.y;y < size.y;y += wiredistance)
		{
			meshBuilder.SetUV(Vector2(0, 0));
			meshBuilder.SetColor(color);
			meshBuilder.PushVertex(startPosition);

			meshBuilder.SetUV(Vector2(0, 1));
			meshBuilder.SetColor(color);
			meshBuilder.PushVertex(endPosition);

			startPosition.y += wiredistance;
			endPosition.y	+= wiredistance;

			meshBuilder.AddTriangleIndex(index, index + 1, index + 2);
			index += 3;
			
		}
		startPosition.y = frontBottomLeftCorner.y;
		endPosition.y   = frontBottomRigthCorner.y;
		startPosition.z += wiredistance;
		endPosition.z   += wiredistance;
	}

	meshBuilder.m_indices.push_back(0);
	meshBuilder.End();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : Creates a 2d Grid of lines
*
*@param   : position,dimensions and color
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::Create2DGrid(MeshBuilder &meshBuilder,Vector3 position, Vector2 dimensions,Vector3 rightDirection,Vector3 upDirection, Rgba color)
{
	meshBuilder.Begin(PRIMITIVE_LINES, true);
	int vertexIndex = 0;
	for(float index = -dimensions.x;index <= dimensions.x;index++)
	{
		Vector3 currentPositionCentre = position - rightDirection * index;
		Vector3 lineStartPosition = currentPositionCentre - upDirection * dimensions.y;
		Vector3 lineEndPosition   = currentPositionCentre + upDirection * dimensions.y;

		meshBuilder.SetUV(Vector2(0, 0));
		meshBuilder.SetColor(color);
		meshBuilder.PushVertex(lineStartPosition);

		meshBuilder.SetUV(Vector2(0, 1));
		meshBuilder.SetColor(color);
		meshBuilder.PushVertex(lineEndPosition);
		meshBuilder.m_indices.push_back(vertexIndex);
		meshBuilder.m_indices.push_back(vertexIndex +1);
		vertexIndex += 2;
	}
	for (float index = -dimensions.y; index <= dimensions.y; index++)
	{
		Vector3 currentPositionCentre = position - upDirection * index;
		Vector3 lineStartPosition = currentPositionCentre - rightDirection * dimensions.x;
		Vector3 lineEndPosition   = currentPositionCentre + rightDirection * dimensions.x;

		meshBuilder.SetUV(Vector2(0, 0));
		meshBuilder.SetColor(color);
		meshBuilder.PushVertex(lineStartPosition);

		meshBuilder.SetUV(Vector2(0, 1));
		meshBuilder.SetColor(color);
		meshBuilder.PushVertex(lineEndPosition);
		meshBuilder.m_indices.push_back(vertexIndex);
		meshBuilder.m_indices.push_back(vertexIndex + 1);
		vertexIndex += 2;
	}
	meshBuilder.End();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/12
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::CreateBasis(MeshBuilder &meshBuilder, Vector3 position, Vector3 rotation, float scale)
{
	meshBuilder.Begin(PRIMITIVE_LINES, true);
	Transform basisTransform;
	basisTransform.RotateLocalByEuler(rotation);

	Vector3 rigthDirection   = basisTransform.GetLocalMatrix().GetIVector();
	Vector3 UpDirection      = basisTransform.GetLocalMatrix().GetJVector();
	Vector3 forwardDirection = basisTransform.GetLocalMatrix().GetKVector();

	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetColor(Rgba::RED);
	meshBuilder.PushVertex(position);

	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetColor(Rgba::RED);
	meshBuilder.PushVertex(position + rigthDirection*scale);
	meshBuilder.m_indices.push_back(0);
	meshBuilder.m_indices.push_back(1);
	//////////////////////////////////////////////////////////////////////////
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetColor(Rgba::GREEN);
	meshBuilder.PushVertex(position);

	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetColor(Rgba::GREEN);
	meshBuilder.PushVertex(position + UpDirection * scale);
	meshBuilder.m_indices.push_back(2);
	meshBuilder.m_indices.push_back(3);
	//////////////////////////////////////////////////////////////////////////
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetColor(Rgba::BLUE);
	meshBuilder.PushVertex(position);

	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetColor(Rgba::BLUE);
	meshBuilder.PushVertex(position + forwardDirection * scale);
	meshBuilder.m_indices.push_back(4);
	meshBuilder.m_indices.push_back(5);
	//////////////////////////////////////////////////////////////////////////
	meshBuilder.End();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/20
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::CreateLineSet(MeshBuilder &meshBuilder, Vector3 startPosition[], Vector3 endPosition[], int count,Rgba color)
{
	meshBuilder.Begin(PRIMITIVE_LINES, true);
	int indices = 0;
	for(int lineIndex = 0;lineIndex < count;lineIndex++)
	{
		meshBuilder.SetUV(Vector2(0, 0));
		meshBuilder.SetColor(color);
		meshBuilder.PushVertex(startPosition[lineIndex]);

		meshBuilder.SetUV(Vector2(0, 1));
		meshBuilder.SetColor(color);
		meshBuilder.PushVertex(endPosition[lineIndex]);

		meshBuilder.m_indices.push_back(indices++);
		meshBuilder.m_indices.push_back(indices++);
	}
	meshBuilder.End();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/01
*@purpose : Creates triangle with points given
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MeshBuilder::CreateTriangle(MeshBuilder &meshBuilder, Vector3 position1, Vector3 position2, Vector3 position3, Rgba color)
{
	//int indices = static_cast<int>(meshBuilder.m_indices.size());
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetColor(color);
	meshBuilder.PushVertex(position1);

	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetColor(color);
	meshBuilder.PushVertex(position2);

	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetColor(color);
	meshBuilder.PushVertex(position3);

	//meshBuilder.AddTriangleIndex(indices,indices + 1,indices + 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MeshBuilder::CreateRect(MeshBuilder &meshBuilder, Vector3 position1, Vector3 position2, Vector3 position3, Vector3 position4, Rgba color)
{
//	int qindex = static_cast<int>(meshBuilder.m_indices.size());
	meshBuilder.SetUV(Vector2(0, 0));
	meshBuilder.SetColor(color);
	meshBuilder.PushVertex(position1);

	meshBuilder.SetUV(Vector2(0, 1));
	meshBuilder.SetColor(color);
	meshBuilder.PushVertex(position2);

	meshBuilder.SetUV(Vector2(1, 1));
	meshBuilder.SetColor(color);
	meshBuilder.PushVertex(position3);

	meshBuilder.SetUV(Vector2(1, 0));
	meshBuilder.SetColor(color);
	meshBuilder.PushVertex(position4);
	//meshBuilder.AddQuadIndex(qindex, qindex + 1, qindex + 3, qindex + 2);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/04
*@purpose : Creates mesh from file
*
*@param   : file path
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Mesh * MeshBuilder::CreateMeshFromFile(std::string imageFilePath)
{
	Mesh *mesh = new Mesh();
	MeshBuilder *meshBuilder = new MeshBuilder();
	meshBuilder->Begin(PRIMITIVE_TRIANGES, true);
	char *src = (char*)FileReadToBuffer(imageFilePath.c_str());
	/*
	 v  -1.4616 0.9576 -1.1791
	 vn -0.0003 1.0000 0.0006
	 vt 0.6668 0.4638 0.0000
	 f 1/1/1 2/2/1 3/3/1 4/4/1
	 */
	static int triangle = 0;
	static int poly = 0;
	while(*src != NULL)
	{
		std::string oneLine = ReadLine(src);
		src = src + oneLine.length() + 1;

		if(oneLine.size() == 0)
		{
			continue;
		}
		if(StartsWith(oneLine,"v "))
		{
			oneLine = oneLine.substr(1, oneLine.length());
			oneLine = TrimString(oneLine);
			std::vector<std::string> vertices;
			Split(vertices,oneLine, ' ');
			float xPos = std::stof(vertices.at(0), 0);
			float yPos = std::stof(vertices.at(1), 0);
			float zPos = std::stof(vertices.at(2), 0);
			Vector3 position(-1*xPos, yPos,zPos);
			mesh->m_vertices.push_back(position);
		}

		if(StartsWith(oneLine,"vn"))
		{
			oneLine = oneLine.substr(2, oneLine.length());
			oneLine = TrimString(oneLine);
			std::vector<std::string> normals;
			Split(normals,oneLine, ' ');
			float xPos = std::stof(normals.at(0), 0);
			float yPos = std::stof(normals.at(1), 0);
			float zPos = std::stof(normals.at(2), 0);
			Vector3 normal(xPos, yPos, zPos);
			mesh->m_normal.push_back(normal);
		}

		if(StartsWith(oneLine,"vt"))
		{
			oneLine = oneLine.substr(2, oneLine.length());
			oneLine = TrimString(oneLine);
			std::vector<std::string> uvs;
			Split(uvs,oneLine, ' ');
			float xPos = std::stof(uvs.at(0), 0);
			float yPos = std::stof(uvs.at(1), 0);
			Vector2 uv(xPos, yPos);
			mesh->m_uvs.push_back(uv);
		}

		if(StartsWith(oneLine, "f"))
		{
			oneLine = oneLine.substr(1, oneLine.length());
			oneLine = TrimString(oneLine);
			int faceCount = static_cast<int>(meshBuilder->m_vertices.size());
			std::vector<std::string> vertexPCUN;
			Split(vertexPCUN,oneLine, ' ');
			int count =  static_cast<int>(vertexPCUN.size());
			for (int faceIndex = 0; faceIndex < count; faceIndex++)
			{
				std::vector<std::string> vertexElements;
				Split(vertexElements,vertexPCUN.at(faceIndex), '/');
				int positionIndex = 0;
				int uvIndex       = 0;
				int normalIndex   = 0;
				ToInt(vertexElements.at(0),&positionIndex);
				ToInt(vertexElements.at(1),&uvIndex);
				ToInt(vertexElements.at(2),&normalIndex);
				//INDEX STARTS FROM 1 so decrement value by 1
				meshBuilder->SetUV(mesh->m_uvs.at(uvIndex - 1));
				meshBuilder->SetNormal(mesh->m_normal.at(normalIndex -1));
				Vector3 position = mesh->m_vertices.at(positionIndex-1);
				Vector3 tangent(0, 0, 0);
				Vector3 bitangent(0, 0, 0);
				meshBuilder->GenerateArbitaryTangent(&tangent, &bitangent, mesh->m_normal.at(normalIndex - 1));
				meshBuilder->SetTangent(tangent);
				meshBuilder->SetBiTangent(bitangent);
				meshBuilder->SetColor(Rgba::WHITE);
				meshBuilder->PushVertex(position);
			}
			if(count == 3)
			{
				triangle++;
				meshBuilder->AddTriangleIndex(faceCount, faceCount + 1, faceCount + 2);
			}
			else
			{
				poly++;
				meshBuilder->AddQuadIndex(faceCount, faceCount + 1, faceCount + 3, faceCount + 2); // anticlockwise
			}
		}
	}
	meshBuilder->End();
	return meshBuilder->CreateMesh();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/22
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool MeshBuilder::GenerateMikkt(MeshBuilder &meshBuilder)
{
	UNUSED(meshBuilder);
	return true;

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/25
*@purpose : Generate and arbitary tangent 
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void MeshBuilder::GenerateArbitaryTangent(Vector3 *tangent, Vector3 *bitangent, Vector3 normal)
{
	Vector3 right = CrossProduct(normal, Vector3(0,1,0));

	if (right.IsNearZero())
	{
		right = CrossProduct(normal, Vector3(1,0,0));
	}
	right = right.GetNormalized();

	Vector3 up = CrossProduct(right, normal);
	up = up.GetNormalized();

	*tangent = right;
	*bitangent = up;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/01
*@purpose : Removes vertices from begin of mb
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MeshBuilder::RemoveVerticesFromBegin(int count)
{
	m_vertices.erase(m_vertices.begin(), m_vertices.begin() + count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/01
*@purpose : removes indices from begin
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MeshBuilder::RemoveIndicesFromBegin(int count)
{
	m_indices.erase(m_indices.begin(), m_indices.begin() + count);
}
