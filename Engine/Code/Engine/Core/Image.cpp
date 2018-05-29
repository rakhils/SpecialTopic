#include "Image.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Image::Image()
{

}

Image::Image(const std::string& imageFilePath)
{
	m_filePath = imageFilePath;
	texture = Texture::CreateOrGetTexture(imageFilePath,true,true);
	m_dimensions = texture->getDimensions();
}
	
//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/05
*@purpose : Default constructor
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
Image::Image(Rgba rgba)
{
	m_dimensions.x = 1;
	m_dimensions.y = 1;
	m_texels.push_back(rgba);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
Image::Image(const std::string& imageFilePath, bool storeData, bool flipY /*= true*/)
{
	m_filePath = imageFilePath;
	texture = Texture::CreateOrGetTexture(imageFilePath, storeData, flipY);
	m_dimensions = texture->getDimensions();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
const Rgba* Image::GetTexelReference(int x, int y) const
{
	int index = m_dimensions.x * y + x;
	return texture->pixels[index];
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
Rgba* Image::GetTexelReference(int x, int y)
{
	if(x >= m_dimensions.x  || y >= m_dimensions.y || x < 0 || y < 0)
	{
		GUARANTEE_OR_DIE(false, "BAD");
	}
	int index = m_dimensions.x * y + x;
	return &texture->texels[index];
}

//////////////////////////////////////////////////////////////////////////
//@purpose : Get pixel by row column
Rgba Image::GetTexel(int x, int y) const
{
	return texture->getTexel(x,y);
}

//////////////////////////////////////////////////////////////////////////
//@purpose : Get pixel by array index
Rgba Image::GetTexelByIndex(int index) const
{
	return texture->getTexel(index);
}

void Image::SetTexel(int x, int y, const Rgba& color)
{
	texture->setTexel(x,y,color);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Get texels as char array
*@param   : NIL
*@return  : All texels as pointer
*//////////////////////////////////////////////////////////////
unsigned char* Image::GetTexels()
{
	return (unsigned char*)m_texels.data();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
std::string Image::GetFilePath()
{
	return m_filePath;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/08
*@purpose : Returns dimensions
*@param   : NIL
*@return  : dimensions
*//////////////////////////////////////////////////////////////
IntVector2 Image::GetDimensions()
{
	return m_dimensions;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
bool Image::LoadFromFile(std::string filename)
{
	texture = Texture::CreateOrGetTexture(filename, false);
	return true;
}
