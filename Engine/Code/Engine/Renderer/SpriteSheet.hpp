#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/AABB2.hpp"

class SpriteSheet
{

public:

	SpriteSheet( const Texture& texture, int tilesWide, int tilesHigh );

	AABB2 GetTexCoordsForSpriteCoords( const IntVector2& spriteCoords ) const; // for sprites
	AABB2 GetTexCoordsForSpriteIndex( int spriteIndex ) const; // for sprite animations
	int   GetNumSprites() const;
	const Texture* getTexture();
	IntVector2 GetSpriteLayout();
	
private:
	const Texture& 	m_spriteSheetTexture; 	// Texture w/grid-based layout of sprites
	IntVector2		m_spriteLayout;		// # of sprites across, and down, on the sheet
									
};
