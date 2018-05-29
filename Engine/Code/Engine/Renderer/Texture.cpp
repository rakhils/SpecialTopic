#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/GL/glfunctions.hpp"
#include "Engine/ThirdParty/stb/stb_image.h"
#include "Engine/Core/EngineCommon.hpp"
//#include "Engine/Renderer/Renderer.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Texture *Texture::s_currentTexture		= nullptr;
Texture *Texture::s_defaultTexture		= nullptr;
Texture *Texture::s_defaultColorTarget	= nullptr;
Texture *Texture::s_defaultDepthTarget	= nullptr;
Texture *Texture::s_effectCurrentTarget = nullptr;
Texture *Texture::s_effectScratch		= nullptr;


std::map< std::string, Texture *> Texture::m_textureMap;
Texture::Texture( const std::string& imageFilePath ,bool storeData,bool flipY)
	: m_textureID( 0 )
	, m_dimensions(0, 0 )
{
	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)

	if(flipY)
	{
		stbi_set_flip_vertically_on_load(flipY);
	}
	this->imageData = stbi_load( imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested );
	m_numOfComponents = numComponents;
	PopulateFromData( imageData, m_dimensions, numComponents );
	if (flipY)
	{
		stbi_set_flip_vertically_on_load(false);
	}
	if(storeData)
	{
		storeAllData();
	}
	stbi_image_free(imageData);
	//glGenTextures( 1, &m_texture_handler );
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/05
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Texture::Texture(Image image)
{
	int numComponents = 4; //RGBA
	m_dimensions = image.GetDimensions();
	unsigned char* imgData = image.GetTexels();
	PopulateFromData(imgData, m_dimensions, numComponents);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Texture::Texture()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Texture::Texture(const std::string& imageFilePath)
{
	Texture(imageFilePath,false);
	m_imageFilePath = imageFilePath;
}

//-----------------------------------------------------------------------------------------------
// Creates a texture identity on the video card, and populates it with the given image texel data
//
void Texture::PopulateFromData( unsigned char* imgData, const IntVector2& texelSize, int numComponents )
{
	m_dimensions = texelSize;

	// Enable texturing
	//glEnable( GL_TEXTURE_2D );

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*) &m_textureID );

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, m_textureID );

	// Set texture clamp vs. wrap (repeat)
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_CLAMP or GL_REPEAT
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // GL_CLAMP or GL_REPEAT
	////
	////// Set magnification (texel > pixel) and minification (texel < pixel) filters
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if( numComponents == 3 )
		bufferFormat = GL_RGB;

	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL

	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		m_dimensions.x,			// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
		m_dimensions.y,			// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1, recommend 0)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
		imgData );		// Address of the actual pixel data bytes/buffer in system memory
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/03
*@purpose : Returns handle 
*
*@param   : NIL
*
*@return  : handler
*/
//////////////////////////////////////////////////////////////
GLuint Texture::GetHandle()
{
	return m_textureID;
}

Rgba Texture::getTexel(int index)
{
	return *(pixels.at(index));
}

Rgba Texture::getTexel(int x,int y)
{
	int pixelStart = y*m_dimensions.x + x;
	return *(pixels.at(pixelStart));
}

void Texture::setTexel(int x, int y, Rgba rgba)
{
	unsigned char r = rgba.r;
	unsigned char g = rgba.g;
	unsigned char b = rgba.b;
	unsigned char a = rgba.a;
	Rgba *rgb;
	rgb = new Rgba(r,g,b,a);
	int pixelStart = y*m_dimensions.x + x;
	pixels[pixelStart] = rgb;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/05
*@purpose : Setting dimensions of an image
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Texture::SetDimensions(float x,float y)
{
	m_dimensions.x = static_cast<int>(x);
	m_dimensions.y = static_cast<int>(y);
	for(int index = 0;index<x*y;index++)
	{
		Rgba *rgba = new Rgba();
		rgba->SetAsFloats(1.0f,1.0f,1.0f,1.0f);
		pixels.push_back(rgba);
	}
}

void Texture::clearAllData()
{
	stbi_image_free( imageData );
}

void Texture::storeAllData()
{
	for(int i = 0,j=0;i<m_numOfComponents*m_dimensions.x*m_dimensions.y;i+=m_numOfComponents,j++)
	{
		Rgba *rgba;
		rgba = new Rgba(imageData[i],imageData[i+1],imageData[i+2],imageData[i+3]);

		Rgba color(imageData[i], imageData[i + 1], imageData[i + 2], imageData[i + 3]);
		pixels.push_back(rgba);
		texels.push_back(color);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/18
*@purpose : For getting texture dimensions
*
*@param   : NIL
*
*@return  : Return dimension of current texture
*/
//////////////////////////////////////////////////////////////

IntVector2 Texture::getDimensions() const
{
	return m_dimensions;
}


bool Texture::CreateRenderTarget(int width, int height, eTextureFormat fmt)
{
	// generate the link to this texture
	glGenTextures(1, &m_textureID);
	if (m_textureID == NULL) 
	{
		return false;
	}

	// TODO - add a TextureFormatToGLFormats( GLenum*, GLenum*, GLenum*, eTextureFormat )
	//        when more texture formats are required; 
	GLenum internal_format = GL_RGBA8;
	GLenum channels        = GL_RGBA;
	GLenum pixel_layout    = GL_UNSIGNED_BYTE;
	if (fmt == TEXTURE_FORMAT_D24S8) 
	{
		internal_format = GL_DEPTH_STENCIL;
		channels	    = GL_DEPTH_STENCIL;
		pixel_layout    = GL_UNSIGNED_INT_24_8;
	}

	// Copy the texture - first, get use to be using texture unit 0 for this; 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);    // bind our texture to our current texture unit (0)

												 // Copy data into it;
	glTexImage2D(GL_TEXTURE_2D, 0,
		internal_format, // what's the format OpenGL should use
		width,
		height,
		0,             // border, use 0
		channels,      // how many channels are there?
		pixel_layout,  // how is the data laid out
		nullptr);     // don't need to pass it initialization data 

					   // make sure it suceeded
	
	// cleanup after myself; 
	GLCheckError(__FILE__,__LINE__);
	glBindTexture(GL_TEXTURE_2D, NULL); // unset it; 

										  // Save this all off
	m_dimensions.x = width;
	m_dimensions.y = height;

	m_format = fmt; // I save the format with the texture
					// for sanity checking.

					// great, success
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/21
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Texture* Texture::Clone(Texture* original)
{
	Texture *texture = new Texture();
	texture->m_dimensions = original->getDimensions();
	return texture;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/22
*@purpose : Creates or retrieves texture
*@param   : Image file path, store or not,flipY enable or disable
*@return  : Texture pointer
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Texture* Texture::CreateOrGetTexture(const std::string& imageFilePath, bool shouldStore, bool flipY)
{
	if (m_textureMap.find(imageFilePath) != m_textureMap.end())
	{
		return m_textureMap[imageFilePath];
	}
	if (imageFilePath == "default")
	{
		Texture *texture = new Texture();
		texture->SetDimensions(1, 1);
		texture->CreateRenderTarget(1, 1, TEXTURE_FORMAT_D24S8);
		m_textureMap[imageFilePath] = texture;
		return texture;
	}
	Texture *texture = new Texture(imageFilePath, shouldStore, flipY);
	m_textureMap[imageFilePath] = texture;
	return texture;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/22
*@purpose : Creates or retrieves texture
*@param   : Image file path, store or not,flipY enable or disable
*@return  : Texture pointer
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Texture* Texture::CreateOrGetTexture(const std::string& imageFilePath)
{
	return CreateOrGetTexture(imageFilePath, true, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/22
*@purpose : Creates or retrieves texture
*@param   : Image file path, store or not,flipY enable or disable
*@return  : Texture pointer
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Texture* Texture::CreateOrGetTexture(const std::string& imageFilePath, bool shouldStore)
{
	return CreateOrGetTexture(imageFilePath, shouldStore, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/22
*@purpose : Create or get texture from image object
*@param   : Image object
*@return  : Texture pointer
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Texture* Texture::CreateOrGetTexture(Image &image)
{
	if (m_textureMap.find(image.GetFilePath()) != m_textureMap.end())
	{
		return m_textureMap[image.GetFilePath()];
	}
	Texture *texture = image.texture;
	m_textureMap[image.GetFilePath()] = texture;
	return texture;
}

