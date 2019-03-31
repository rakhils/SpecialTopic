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

class Tiles
{
public:

	TILE_TYPE		m_type = FREE;
	IntVector2		m_tileCoords;
	AABB2			m_aabb2;
	TileDefinition *m_tileDef = nullptr;
	SpriteSheet *	spriteSheet;

	Tiles();

	void Render();
};
