#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "TileDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "TileDefinition.hpp"

class Tiles
{
public:
	std::string m_type;
	Vector2 m_tileCoords;
	float m_centreX;
	float m_centreY;
	float m_height = 1;
	float m_width = 1;
	AABB2 m_aabb2;
	TileDefinition *m_tileDef = nullptr;
	SpriteSheet *spriteSheet;
	Tiles(std::string type);
	

	void Render();
};
