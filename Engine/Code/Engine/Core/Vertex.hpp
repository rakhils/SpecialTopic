#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/Layouts/VertexLayout.hpp"

struct VertexMaster
{
	Vector3 m_position;
	Vector2 m_uvs;
	Vector3 m_normal;
	Vector3 m_tangent   = Vector3(1,0,0);
	Vector3 m_bitangent = Vector3(0,0,1);
	Rgba    m_rgba;

	static VertexAttribute  s_ATTRIBUTES[];
	static VertexLayout     s_LAYOUT;
};

struct Vertex_3DPCUNTB
{
	Vector3 m_position;
	Vector2 m_uvs;
	Vector3 m_normal;
	Vector3 m_tangent;
	Vector3 m_bitangent;
	Rgba    m_rgba;
	Vertex_3DPCUNTB() {};
	Vertex_3DPCUNTB(VertexMaster vertexMaster)
	{
		m_position = vertexMaster.m_position;
		m_rgba = vertexMaster.m_rgba;
		m_uvs = vertexMaster.m_uvs;
		m_normal = vertexMaster.m_normal;
		m_bitangent = vertexMaster.m_bitangent;
		m_tangent = vertexMaster.m_tangent;
	}
	Vertex_3DPCUNTB(VertexMaster *vertexMaster)
	{
		m_position = vertexMaster->m_position;
		m_rgba = vertexMaster->m_rgba;
		m_uvs = vertexMaster->m_uvs;
		m_normal = vertexMaster->m_normal;
	}

	Vertex_3DPCUNTB(Vector3 point)
	{
		m_position = point;
	}
	void SetPoint(Vector3 point)
	{
		m_position = point;
	}
	void SetPoint(float x, float y, float z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
	}
	void SetPoint(float x, float y)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = 0;
	}

	void SetRGBA(Rgba rgba)
	{
		m_rgba = rgba;
	}

	void SetUV(Vector2 uv)
	{
		m_uvs = uv;
	}

	void SetNormal(Vector3 normal)
	{
		m_normal = normal;
	}
	static VertexAttribute  s_ATTRIBUTES[];
	static VertexLayout     s_LAYOUT;
};

struct Vertex_3DPCUN
{
	Vector3 m_position;
	Vector2 m_uvs;
	Vector3 m_normal;
	Rgba    m_rgba;
	Vertex_3DPCUN() {};
	Vertex_3DPCUN(VertexMaster vertexMaster)
	{
		m_position = vertexMaster.m_position;
		m_rgba     = vertexMaster.m_rgba;
		m_uvs      = vertexMaster.m_uvs;
		m_normal   = vertexMaster.m_normal;
	}
	Vertex_3DPCUN(VertexMaster *vertexMaster)
	{
		m_position = vertexMaster->m_position;
		m_rgba = vertexMaster->m_rgba;
		m_uvs = vertexMaster->m_uvs;
		m_normal = vertexMaster->m_normal;
	}

	Vertex_3DPCUN(Vector3 point)
	{
		m_position = point;
	}
	void SetPoint(Vector3 point)
	{
		m_position = point;
	}
	void SetPoint(float x, float y, float z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
	}
	void SetPoint(float x, float y)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = 0;
	}

	void SetRGBA(Rgba rgba)
	{
		m_rgba = rgba;
	}

	void SetUV(Vector2 uv)
	{
		m_uvs = uv;
	}

	void SetNormal(Vector3 normal)
	{
		m_normal = normal;
	}
	static VertexAttribute  s_ATTRIBUTES[];
	static VertexLayout     s_LAYOUT;
};

struct Vertex_3DPCU 
{
public:
	
	Vector3 m_position;
	Vector2 m_uvs;
	Rgba m_rgba;

	Vertex_3DPCU(){};
	Vertex_3DPCU(VertexMaster vertexMaster)
	{
		m_position = vertexMaster.m_position;
		m_rgba     = vertexMaster.m_rgba;
		m_uvs	   = vertexMaster.m_uvs;
	}
	Vertex_3DPCU(VertexMaster *vertexMaster)
	{
		m_position = vertexMaster->m_position;
		m_rgba = vertexMaster->m_rgba;
		m_uvs = vertexMaster->m_uvs;
	}
	Vertex_3DPCU(Vector3 point)
	{
		m_position = point;
	}
	void setPoint(Vector3 point)
	{
		m_position = point;
	}
	void setPoint(float x,float y, float z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
	}
	void setPoint(float x, float y)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = 0;
	}

	void setRGBA(Rgba rgba)
	{
		m_rgba = rgba;
	}

	void setUV(Vector2 uv)
	{
		m_uvs = uv;
	}

	static VertexAttribute  s_ATTRIBUTES[];
	static VertexLayout     s_LAYOUT;
};

