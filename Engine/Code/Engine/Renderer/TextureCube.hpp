#pragma once

#include "Engine/Renderer/Renderer.hpp"

class Image;  // core/image/image.h

class TextureCube
{
public:
	TextureCube();
	virtual ~TextureCube();

	void cleanup();

	// make a cube map from 6 images
	// +x -x +y -y +z -z
	bool make_from_images(Image *images);

	// todo - may want this to take tile offsets
	// into the image since I can't assume the same
	// plus sign format my demo one is in
	bool make_from_image(Image &image);
	bool make_from_image( char const *filename ); 

	// cube maps should be square on each face
	inline int get_width() const { return m_size; }
	inline int get_height() const { return m_size; }

	inline bool is_valid() const { return (m_handle != NULL); }


	static void FlipAndBindImage(eTextureCubeSide side, Image const &img, GLenum channels, GLenum pixel_layout);
	static void BindImageToSide(eTextureCubeSide side, Image &img, int size, int ox, int oy, GLenum channels, GLenum pixel_layout);
	static Image MassageImageForTexture(Image &src);


public:
	
	inline int get_handle() const { return m_handle; }

public:
	int m_size;          // cube maps are equal length on all sizes
	eTextureFormat m_format;

	// GL 
	GLuint m_handle;

public:
};
