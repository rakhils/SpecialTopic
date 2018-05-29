#pragma once
#include <string>
#include <vector>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"
//#include "Engine/Renderer/Texture.hpp"
class Texture;
class Image
{
public:
	Texture *texture;
	explicit Image( const std::string& imageFilePath );
	Image(const std::string& imageFilePath, bool storeData, bool flipY = true);
	Image(Rgba rgba);
	Image();

public:
	Rgba			GetTexelByIndex( int index ) const;
	const Rgba*		GetTexelReference( int x, int y ) const;
	Rgba*			GetTexelReference(int x, int y);
	Rgba			GetTexel( int x, int y ) const; // (0,0) is top-left
	void			SetTexel( int x, int y, const Rgba& color );
	unsigned char*  GetTexels();
	std::string		GetFilePath();
	IntVector2		GetDimensions();
	bool			LoadFromFile(std::string filename);

private:
	std::string m_filePath;
	IntVector2 m_dimensions;
	std::vector< Rgba >m_texels; // ordered left-to-right, then down, from [0]=(0,0) at top-left

};