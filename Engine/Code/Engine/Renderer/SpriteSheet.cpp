#include "Engine/Renderer/SpriteSheet.hpp"


SpriteSheet::SpriteSheet(const Texture& texture, int tilesWide, int tilesHigh):m_spriteSheetTexture(texture)
{
	m_spriteLayout.x = tilesWide;
	m_spriteLayout.y = tilesHigh;
}

AABB2 SpriteSheet::GetTexCoordsForSpriteCoords(const IntVector2& spriteCoords) const
{
	float BottomLeftX = (spriteCoords.x       * 1.0f/m_spriteLayout.x);
	float BottomLeftY = (spriteCoords.y       * 1.0f/m_spriteLayout.y);
	float TopRightY   = ((spriteCoords.y + 1) * 1.0f/m_spriteLayout.y);
	float TopRigthX   = ((spriteCoords.x + 1) * 1.0f/m_spriteLayout.x);
	
	//BottomLeftY = m_spriteLayout.y - BottomLeftY;
	//TopRightY = m_spriteLayout.y - TopRightY;
	AABB2 aabb2(BottomLeftX,BottomLeftY,TopRigthX,TopRightY);
	//AABB2 aabb2(BottomLeftX,TopRightY,TopRigthX,BottomLeftY);
	return aabb2;
}

AABB2 SpriteSheet::GetTexCoordsForSpriteIndex(int spriteIndex) const
{
	int posX = spriteIndex % m_spriteLayout.x;
	int posY = spriteIndex / m_spriteLayout.x;
	IntVector2 spriteCoords(posX,posY);
	return GetTexCoordsForSpriteCoords(spriteCoords);
}

int SpriteSheet::GetNumSprites() const
{
	return m_spriteLayout.x*m_spriteLayout.y;
}

const Texture* SpriteSheet::getTexture()
{
	return &m_spriteSheetTexture;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Returns spritelayout
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
IntVector2 SpriteSheet::GetSpriteLayout()
{
	return m_spriteLayout;
}
