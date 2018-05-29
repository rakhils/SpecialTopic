#pragma once
#include <string>

#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Renderer.hpp"

class BitmapFont
{
	friend class Renderer;

public:
	AABB2 GetUVsForGlyph( int glyphUnicode ) const;
	float GetGlyphAspect( int glyphUnicode ) const { int temValue = glyphUnicode;temValue = 1;return m_baseAspect; }
	float GetStringWidth( const std::string& asciiText, float cellHeight, float aspectScale );

	static BitmapFont* CreateOrGetBitmapFont(const char* bitmapFontName);
private:
	explicit BitmapFont( const std::string& fontName,  SpriteSheet& glyphSheet,float baseAspect );
	~BitmapFont();
	std::string m_name;
	float m_baseAspect = 1.0f;
	SpriteSheet *m_spriteSheet = nullptr;

	static std::map< std::string, BitmapFont* > m_loadedFonts;

};