//-----------------------------------------------------------------------------------------------
// Texture.hpp
//
#pragma once
#include <string>
#include <map>
#include <vector>


#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/External/gl/glcorearb.h"
#include "Engine/Core/Image.hpp"

//---------------------------------------------------------------------------
enum eTextureFormat
{
	TEXTURE_FORMAT_RGBA8, // default color format
	TEXTURE_FORMAT_D24S8,
	TEXTURE_FORMAT_UNKNOWN,
};
class Texture
{
	friend class Renderer; // Textures are managed by a Renderer instance
public:
	GLuint			    m_slot = 0U;
	unsigned int		m_textureID;
	IntVector2			m_dimensions;
	int					m_numOfComponents;
	unsigned char*		imageData = nullptr;
	eTextureFormat      m_format;

	std::vector <Rgba*> pixels;
	std::vector<Rgba> texels;
	static std::map< std::string, Texture *> m_textureMap;
public:
	Texture();
	Texture(Image image);

private:
	Texture( const std::string& imageFilePath ); // Use renderer->CreateOrGetTexture() instead!
	Texture( const std::string& imageFilePath,bool storeData ,bool flipY = true);
	void PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents );
	void GenerateMipMaps();
	unsigned int  CalculateMipCount(int value);


public:
	GLuint GetHandle();
	Rgba getTexel(int index);
	Rgba getTexel(int x,int y);
	void setTexel(int x,int y,Rgba rgba);
	void SetDimensions(float x,float y);
	void clearAllData();
	void storeAllData();
	IntVector2 getDimensions() const;
	bool	   CreateRenderTarget( int width, int height, eTextureFormat fmt);
	std::string		    m_imageFilePath;
	//STATIC
	static Texture* Clone(Texture*);
	static Texture* CreateOrGetTexture(const std::string& imageFilePath);
	static Texture* CreateOrGetTexture(const std::string& imageFilePath, bool shouldStore);
	static Texture* CreateOrGetTexture(const std::string& imageFilePath, bool shouldStore, bool flipY);
	static Texture* CreateOrGetTexture(Image &image);

	static Texture * GetCurrentTexture()			{ return s_currentTexture;		}
	static Texture * GetDefaultTexture()			{ return s_defaultTexture;		}
	static Texture * GetDefaultColorTargetTexture() { return s_defaultColorTarget;	}
	static Texture * GetDefaultDepthTargetTexture() { return s_defaultDepthTarget;	}
	static Texture * GetDefaultShadowDepthTexture() { return s_defaultShadowDepth;	}
	static Texture * GetEffectCurrentTexture()		{ return s_effectCurrentTarget;	}
	static Texture * GetEffectScratchTexture()		{ return s_effectScratch;		}

	static void SetCurrentTexture(Texture *texture)              {s_currentTexture		= texture;}
	static void SetDefaultTexture(Texture *texture)              {s_defaultTexture		= texture;}
	static void SetDefaultColorTargetTexture(Texture *texture)   {s_defaultColorTarget  = texture;}
	static void SetDefaultDepthTargetTexture(Texture *texture)   {s_defaultDepthTarget  = texture;}
	static void SetDefaultShadowDepthTexture(Texture *texture)   {s_defaultShadowDepth  = texture;}
	static void SetEffectCurrentTexture(Texture *texture)        {s_effectCurrentTarget = texture;}
	static void SetEffectScratchTexture(Texture *texture)        {s_effectScratch		= texture;}

private:
	static Texture *s_currentTexture;
	static Texture *s_defaultTexture;
	static Texture *s_defaultColorTarget;
	static Texture *s_defaultDepthTarget;
	static Texture *s_defaultShadowDepth;
	static Texture *s_effectCurrentTarget;
	static Texture *s_effectScratch;

};


