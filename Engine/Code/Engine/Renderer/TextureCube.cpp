
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/GL/glfunctions.hpp"
#include "Engine/Renderer/Texture.hpp"

Image TextureCube::MassageImageForTexture(Image &src)
{
	//eTextureFormat tex_format = TEXTURE_FORMAT_RGBA8;// = TextureFormatFromImageFormat(src.get_format());
	//eImageFormat image_format = ImageFormatFromTextureFormat(tex_format);

	/*Image copy;
	if (image_format != src.texture->m_format)
	{
		copy.convert(src, image_format);
	}
	else
	{
		copy.copy(src);
	}*/
	// ImageFlipY( &copy ); // do not flip for CubeMaps - they use a different coordinate system; 
	return src;
}

//------------------------------------------------------------------------
void TextureCube::BindImageToSide(eTextureCubeSide side, Image  &img, int size, int ox, int oy, GLenum channels, GLenum pixel_layout)
{
	//int index = (img).GetDimensions().x * oy  + ox;
	//const void *ptr = (void *)(img.texture->pixels.data() + index); //
	void *ptr = (void *)img.GetTexelReference(ox, oy);
	
	
	glTexSubImage2D(Renderer::ToGLCubeSide(side),0, 0, 0, size, size,channels,pixel_layout,ptr);
}

//------------------------------------------------------------------------
void TextureCube::FlipAndBindImage(eTextureCubeSide side, Image const &img, GLenum channels, GLenum pixel_layout)
{
	UNUSED(pixel_layout);
	UNUSED(channels);
	UNUSED(img);
	UNUSED(side);
	//Image copy = TextureCube::MassageImageForTexture((Image)img);
	//BindImageToSide(side, copy, copy.GetDimensions().x , 0, 0, channels, pixel_layout);
}

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
// TEXTURECUBE
//------------------------------------------------------------------------

//------------------------------------------------------------------------
TextureCube::TextureCube()
{
	m_size = 0U;
	m_format = TEXTURE_FORMAT_RGBA8;
	m_handle = 0U;
}

//------------------------------------------------------------------------
TextureCube::~TextureCube()
{
	cleanup();
}
	
//------------------------------------------------------------------------
void TextureCube::cleanup()
{
	if (is_valid()) 
	{
		glDeleteTextures(1, &m_handle);
		m_handle = NULL;
	}

	m_size = 0;
	m_format = TEXTURE_FORMAT_UNKNOWN;
}

//------------------------------------------------------------------------
// make a cube map from 6 images;
bool TextureCube::make_from_images(Image *images)
{
#if defined(_DEBUG)
	for (int i = 0; i < 6; ++i)
	{
		//ASSERT(images[i].get_width() == images[i].get_height());
		//ASSERT(images[i].get_width() == images[0].get_width());
		//ASSERT(images[i].get_format() == images[0].get_format());
	}
#endif   

	if (m_handle == NULL) 
	{
		glGenTextures(1, &m_handle);
		//ASSERT_RETURN_VALUE(is_valid(), false);
	}

	m_size = images[0].GetDimensions().x;//   get_width();
	m_format = TEXTURE_FORMAT_RGBA8;// (images[0].get_format());

	GLenum internal_format = GL_RGBA8;
	GLenum channels = GL_RGBA;
	GLenum pixel_layout = GL_UNSIGNED_BYTE;
	//TextureGetInternalFormat(&internal_format, &channels, &pixel_layout, m_format);

	// bind it; 
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);

	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, internal_format, m_size, m_size);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	// bind the image to the side; 
	FlipAndBindImage(TEXCUBE_RIGHT,  images[0], channels, pixel_layout);
	FlipAndBindImage(TEXCUBE_LEFT,   images[1], channels, pixel_layout);
	FlipAndBindImage(TEXCUBE_TOP,    images[2], channels, pixel_layout);
	FlipAndBindImage(TEXCUBE_BOTTOM, images[3], channels, pixel_layout);
	FlipAndBindImage(TEXCUBE_FRONT,  images[4], channels, pixel_layout);
	FlipAndBindImage(TEXCUBE_BACK,   images[5], channels, pixel_layout);

	return true;
	//return GLSucceeded();
}

//------------------------------------------------------------------------
// todo - may want this to take tile offsets
// into the image since I can't assume the same
// plus sign format my demo one is in
bool TextureCube::make_from_image(Image &image)
{
	int width = image.GetDimensions().x;
	int size =  width / 4;

	// make sure it is the type I think it is; 
	//ASSERT_RETURN_VALUE(image.get_height() == (size * 3U), false);
	//ASSERT_RETURN_VALUE(image.get_height() == image.get_height(), false);

	if (m_handle == NULL) 
	{
		glGenTextures(1, &m_handle);
		//ASSERT_RETURN_VALUE(is_valid(), false);
	}

	m_size = size;
	//m_format = RgTextureFormatFromImageFormat(image.get_format());
	//Image copy = MassageImageForTexture(image);

	GLenum internal_format = GL_RGBA8;
	GLenum channels = GL_RGBA;
	GLenum pixel_layout = GL_UNSIGNED_BYTE;
	//TextureGetInternalFormat(&internal_format, &channels, &pixel_layout, m_format);
	GL_CHECK_ERROR();

	// bind it; 
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, internal_format, m_size, m_size);
	GL_CHECK_ERROR();

	glPixelStorei(GL_UNPACK_ROW_LENGTH, image.GetDimensions().x);
	GL_CHECK_ERROR();

	// bind the image to the side; 
	BindImageToSide(TEXCUBE_RIGHT, image, m_size, m_size * 2, m_size * 1, channels, pixel_layout);
	BindImageToSide(TEXCUBE_LEFT, image, m_size, m_size * 0, m_size * 1, channels, pixel_layout);
	BindImageToSide(TEXCUBE_TOP, image, m_size, m_size * 1, m_size * 0, channels, pixel_layout);
	BindImageToSide(TEXCUBE_BOTTOM, image, m_size, m_size * 1, m_size * 2, channels, pixel_layout);
	BindImageToSide(TEXCUBE_FRONT, image, m_size, m_size * 1, m_size * 1, channels, pixel_layout);
	BindImageToSide(TEXCUBE_BACK, image, m_size, m_size * 3, m_size * 1, channels, pixel_layout);
	GL_CHECK_ERROR();

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	GL_CHECK_ERROR();

	return true;
}

//------------------------------------------------------------------------
bool TextureCube::make_from_image(char const *filename)
{
	Image image(filename, true, false);
	//if (!image.LoadFromFile(filename))
	//{
	//	return false;
	//}

	return make_from_image(image);
}