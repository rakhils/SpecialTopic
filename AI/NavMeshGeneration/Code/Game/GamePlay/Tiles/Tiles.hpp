#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "TileDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "TileDefinition.hpp"

enum TILE_TYPE
{
	FREE,
	BLOCK
};

struct Node
{
	Vector2 m_position;
	int     m_vertexIndex;
	Node(){}
	Node(Vector2 position)
	{
		m_position = position;
	}
};

struct ControlNode : public Node
{
	bool			m_active = false;
	Node *			m_above  = nullptr;
	Node *			m_right  = nullptr;

	ControlNode() {};
	ControlNode(Vector2 position,bool active,float squreSize)
	{
		m_active = active;
		m_position = position;
		m_above = new Node(position + Vector2(0.f, 1.f)*squreSize/2.f);
		m_right = new Node(position + Vector2(1.f, 0.f)*squreSize/2.f);
	}
};
class Tiles
{
public:

	TILE_TYPE		m_type = FREE;
	IntVector2		m_tileCoords;
	AABB2			m_aabb2;
	TileDefinition *m_tileDef = nullptr;
	SpriteSheet *	spriteSheet;

	ControlNode *m_bottomLeft    = nullptr;
	ControlNode *m_bottomRight   = nullptr;
	ControlNode *m_topRight      = nullptr;
	ControlNode *m_topLeft		 = nullptr;

	Node		*m_centreLeft	 = nullptr;
	Node		*m_centreRight   = nullptr;
	Node		*m_centreTop     = nullptr;
	Node		*m_centreBottom  = nullptr;

	int			 m_configurationNumber = 0;

	Tiles();

	void Render();
	void SetNodes(ControlNode *bottomLeft, ControlNode *bottomRight, ControlNode *topLeft, ControlNode *topRight);

	Vector2 GetTopLeft();
	Vector2 GetTopRight();
	Vector2 GetBottomLeft();
	Vector2 GetBottomRight();

	Vector2 GetCentreLeft();
	Vector2 GetCentreRight();
	Vector2 GetCentreTop();
	Vector2 GetCentreBottom();
};
