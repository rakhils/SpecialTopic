#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "TileDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "TileDefinition.hpp"

enum TileType
{
	BLOCK,
	AIR
};
class Tiles
{
public:

	IntVector2		m_tileCoords;
	AABB2			m_aabb2;
	TileDefinition *m_tileDef = nullptr;
	SpriteSheet *	spriteSheet;
	TileType		m_type;

	Tiles();

	void Render();
};
