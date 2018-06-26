#include "Engine/Core/Vertex.hpp"
VertexAttribute VertexMaster::s_ATTRIBUTES[] =
{
	VertexAttribute("POSITION",    GL_FLOAT,         3, false, offsetof(VertexMaster, m_position)),
	VertexAttribute("COLOR",       GL_UNSIGNED_BYTE, 4, true,  offsetof(VertexMaster, m_rgba)),
	VertexAttribute("UV",          GL_FLOAT,         2, false, offsetof(VertexMaster, m_uvs)),
	VertexAttribute("NORMAL",      GL_FLOAT,         3, true,  offsetof(VertexMaster, m_normal)),
	VertexAttribute("TANGENT",     GL_FLOAT,         3, true,  offsetof(VertexMaster, m_tangent)),
	VertexAttribute("BITANGENT",   GL_FLOAT,         3, true,  offsetof(VertexMaster, m_bitangent)),
	VertexAttribute(), // last one to determine finish
};
VertexLayout  VertexMaster::s_LAYOUT = VertexLayout(sizeof(VertexMaster), VertexMaster::s_ATTRIBUTES);
//////////////////////////////////////////////////////////////////////////

VertexAttribute Vertex_3DPCUNTBT::s_ATTRIBUTES[] =
{
	VertexAttribute("POSITION",    GL_FLOAT,         3, false, offsetof(Vertex_3DPCUNTBT, m_position)),
	VertexAttribute("COLOR",       GL_UNSIGNED_BYTE, 4, true,  offsetof(Vertex_3DPCUNTBT, m_rgba)),
	VertexAttribute("UV",          GL_FLOAT,         2, false, offsetof(Vertex_3DPCUNTBT, m_uvs)),
	VertexAttribute("NORMAL",      GL_FLOAT,         3, true,  offsetof(Vertex_3DPCUNTBT, m_normal)),
	VertexAttribute("TANGENT",     GL_FLOAT,         3, true,  offsetof(Vertex_3DPCUNTBT, m_tangent)),
	VertexAttribute("BITANGENT",   GL_FLOAT,         3, true,  offsetof(Vertex_3DPCUNTBT, m_bitangent)),
	VertexAttribute("TIME",        GL_FLOAT,         1, true,  offsetof(Vertex_3DPCUNTBT, m_time)),
	VertexAttribute(), // last one to determine finish
};

VertexLayout  Vertex_3DPCUNTBT::s_LAYOUT = VertexLayout(sizeof(Vertex_3DPCUNTBT), Vertex_3DPCUNTBT::s_ATTRIBUTES);

//////////////////////////////////////////////////////////////////////////

VertexAttribute Vertex_3DPCUNTB::s_ATTRIBUTES[] =
{
	VertexAttribute("POSITION",    GL_FLOAT,         3, false, offsetof(Vertex_3DPCUNTB, m_position)),
	VertexAttribute("COLOR",       GL_UNSIGNED_BYTE, 4, true,  offsetof(Vertex_3DPCUNTB, m_rgba)),
	VertexAttribute("UV",          GL_FLOAT,         2, false, offsetof(Vertex_3DPCUNTB, m_uvs)),
	VertexAttribute("NORMAL",      GL_FLOAT,         3, true,  offsetof(Vertex_3DPCUNTB, m_normal)),
	VertexAttribute("TANGENT",     GL_FLOAT,         3, true,  offsetof(Vertex_3DPCUNTB, m_tangent)),
	VertexAttribute("BITANGENT",   GL_FLOAT,         3, true,  offsetof(Vertex_3DPCUNTB, m_bitangent)),
	VertexAttribute(), // last one to determine finish
};

VertexLayout  Vertex_3DPCUNTB::s_LAYOUT = VertexLayout(sizeof(Vertex_3DPCUNTB), Vertex_3DPCUNTB::s_ATTRIBUTES);

//////////////////////////////////////////////////////////////////////////

VertexAttribute Vertex_3DPCUN::s_ATTRIBUTES[] =
{
	VertexAttribute("POSITION", GL_FLOAT,         3, false, offsetof(Vertex_3DPCUN, m_position)),
	VertexAttribute("COLOR",    GL_UNSIGNED_BYTE, 4, true,  offsetof(Vertex_3DPCUN, m_rgba)),
	VertexAttribute("UV",       GL_FLOAT,         2, false, offsetof(Vertex_3DPCUN, m_uvs)),
	VertexAttribute("NORMAL",   GL_FLOAT,         3, true,  offsetof(Vertex_3DPCUN, m_normal)),
	VertexAttribute(), // last one to determine finish
};

VertexLayout  Vertex_3DPCUN::s_LAYOUT = VertexLayout(sizeof(Vertex_3DPCUN), Vertex_3DPCUN::s_ATTRIBUTES);

//////////////////////////////////////////////////////////////////////////

VertexAttribute Vertex_3DPCU::s_ATTRIBUTES[] =
{
	VertexAttribute("POSITION", GL_FLOAT,         3, false, offsetof(Vertex_3DPCU, m_position)),
	VertexAttribute("COLOR",    GL_UNSIGNED_BYTE, 4, true,  offsetof(Vertex_3DPCU, m_rgba)),
	VertexAttribute("UV",       GL_FLOAT,         2, false, offsetof(Vertex_3DPCU, m_uvs)),
	VertexAttribute(), // last one to determine finish
};

VertexLayout  Vertex_3DPCU::s_LAYOUT = VertexLayout(sizeof(Vertex_3DPCU), Vertex_3DPCU::s_ATTRIBUTES);