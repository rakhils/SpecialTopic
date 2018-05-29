#include "Engine/Renderer/Sprites.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVector2.hpp"

std::map<std::string, SpriteDefinition*> Sprites::s_spriteDefinition;
std::map< std::string, Sprites *>		 Sprites::m_spritesMap;
//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
SpriteDefinition::SpriteDefinition(XMLElement& element)
{
	/*  <SpriteDefinition id="archer_f.portrait">
	<diffuse src="image/sprite/to_archer.split.png" />
	<ppu count="16" />
	<uv layout="pixel" uvs="152,13,235,130" />
	<pivot xy="0.5,0" />
	</SpriteDefinition>
	*/
	m_id = ParseXmlAttribute(element, "id", m_id);
	XMLElement *childElement = element.FirstChildElement();
	for (; childElement != nullptr; childElement = childElement->NextSiblingElement())
	{
		const char* childValue = childElement->Value();
		std::string childValueStr; childValueStr.append(childValue);
		if (childValueStr == "diffuse")
		{
			m_srcImage = ParseXmlAttribute(*childElement,"src",m_srcImage);
		}

		if (childValueStr == "ppu")
		{
			m_ppu      = ParseXmlAttribute(*childElement,"count",m_ppu);
		}

		if (childValueStr == "uv")
		{
			std::string uvString = "";
			uvString = ParseXmlAttribute(*childElement, "uvs", uvString);
			size_t firstComaIndex = uvString.find(",");
			m_startUV.SetFromText(uvString.substr(0, uvString.find(",", firstComaIndex + 1)).c_str());
			m_endUV.SetFromText(uvString.substr((uvString.find(",", firstComaIndex + 1)+1),uvString.length()).c_str());
		}

		if (childValueStr == "pivot")
		{
			m_pivotPoint = ParseXmlAttribute(*childElement, "xy", m_pivotPoint);
		}
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/20
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
SpriteDefinition::SpriteDefinition()
{

}

// CONSTRUCTOR
//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Sprites::Sprites(SpriteDefinition *def)
{
	m_definition = def;
	Renderer renderer;
	m_texture = Texture::CreateOrGetTexture(m_definition->m_srcImage,false,false);

}

// DESTRUCTOR
Sprites::~Sprites()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
AABB2 Sprites::GetTexCoordsForSpriteIndex()
{
	float minx = static_cast<float>(m_definition->m_startUV.x) / static_cast<float>(m_texture->getDimensions().x);
	float miny = static_cast<float>(m_definition->m_startUV.y) / static_cast<float>(m_texture->getDimensions().y);
	float maxx = static_cast<float>(m_definition->m_endUV.x)  / static_cast<float>(m_texture->getDimensions().x);
	float maxy = static_cast<float>(m_definition->m_endUV.y)  / static_cast<float>(m_texture->getDimensions().y);
	AABB2 aabb2(minx,miny,maxx,maxy);
	return aabb2;
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
AABB2 Sprites::GetTexCoordsForSpriteIndex(IntVector2 minUV, IntVector2 maxUV)
{
	float minx = static_cast<float>(minUV.x) / static_cast<float>(m_texture->getDimensions().x);
	float miny = static_cast<float>(minUV.y) / static_cast<float>(m_texture->getDimensions().y);
	float maxx = static_cast<float>(maxUV.x) / static_cast<float>(m_texture->getDimensions().x);
	float maxy = static_cast<float>(maxUV.y) / static_cast<float>(m_texture->getDimensions().y);
	AABB2 aabb2(minx, miny, maxx, maxy);
	return aabb2;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/11
*@purpose : Loads and stores all definitions
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Sprites::LoadDefinition(std::string xmlPath)
{
	/*  <SpriteDefinition id="archer_f.portrait">
	<diffuse src="image/sprite/to_archer.split.png" />
	<ppu count="16" />
	<uv layout="pixel" uvs="152,13,235,130" />
	<pivot xy="0.5,0" />
	</SpriteDefinition>
	*/
	TinyXMLDocument doc;
	doc.LoadFile(xmlPath.c_str());
	XMLElement *xmlRootElement  = doc.FirstChildElement("SpriteDefinitions");
	XMLElement *xmlChildElement = xmlRootElement->FirstChildElement("SpriteDefinition");
	for (; xmlChildElement != nullptr; xmlChildElement = xmlChildElement->NextSiblingElement())
	{
		SpriteDefinition *spriteDef = new SpriteDefinition(*xmlChildElement);
		s_spriteDefinition[spriteDef->m_id] = spriteDef;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : Retuns definitions
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
SpriteDefinition* Sprites::GetDefinition(std::string name)
{
	return s_spriteDefinition[name];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/22
*@purpose : Creates or retrieves sprites from static container
*@param   : sprite id
*@return  : sprite pointer
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Sprites * Sprites::CreateOrGetSprites(const std::string& spriteID)
{
	if (m_spritesMap.find(spriteID) != m_spritesMap.end())
	{
		return m_spritesMap[spriteID];
	}
	Sprites *sprites = new Sprites(Sprites::GetDefinition(spriteID));
	m_spritesMap[spriteID] = sprites;
	return sprites;
}

