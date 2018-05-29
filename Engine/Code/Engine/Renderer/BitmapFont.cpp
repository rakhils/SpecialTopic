#include "BitmapFont.hpp"
std::map< std::string, BitmapFont* > BitmapFont::m_loadedFonts;

BitmapFont::BitmapFont(const std::string& fontName, SpriteSheet& glyphSheet, float baseAspect) 
	:m_spriteSheet(&glyphSheet), m_name(fontName)
{
	m_baseAspect = baseAspect;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BitmapFont::~BitmapFont()
{
	delete m_spriteSheet;
}

AABB2 BitmapFont::GetUVsForGlyph(int glyphUnicode) const
{
	int width  = m_spriteSheet->getTexture()->getDimensions().x;
	int heigth = m_spriteSheet->getTexture()->getDimensions().y;

	int posX = glyphUnicode%width;
	int posY = glyphUnicode/heigth;

	IntVector2 spriteCoords(posX,posY);
	IntVector2 m_spriteLayout(width,heigth);

	float BottomLeftX = (spriteCoords.x       * 1.0f/m_spriteLayout.x);
	float BottomLeftY = (spriteCoords.y		  * 1.0f/m_spriteLayout.y);
	float TopRigthX   = ((spriteCoords.x + 1) * 1.0f/m_spriteLayout.x);
	float TopRightY   = ((spriteCoords.y + 1) * 1.0f/m_spriteLayout.y);
	AABB2 aabb2(BottomLeftX,BottomLeftY,TopRigthX,TopRightY);

	return aabb2;
}

float BitmapFont::GetStringWidth(const std::string& asciiText, float cellHeight, float aspectScale)
{
	return asciiText.length()*aspectScale*cellHeight;
}

BitmapFont* BitmapFont::CreateOrGetBitmapFont(const char* bitmapFontName)
{
	std::string path(bitmapFontName);
	if (m_loadedFonts[path] == nullptr)
	{
		Texture*     texture    = Texture::CreateOrGetTexture(path);
		SpriteSheet* fontSheet  = new SpriteSheet(*texture, 16, 16);
		BitmapFont*  bitMapFont = new BitmapFont(bitmapFontName, *fontSheet, 1);
		m_loadedFonts[path] = bitMapFont;
	}
	return m_loadedFonts[path];
}

