#include "Game/GamePlay/Entity/EntityDefinition.hpp"
#include "Engine/TinyXml2/tinyxml2.h"
#include "Engine/Core/XMLUtilities.hpp"

std::map<std::string, EntityDefinition*> EntityDefinition::s_definitions;
std::map<std::string, EntityDefinition*> EntityDefinition::s_RGBDefinition;
// CONSTRUCTOR

EntityDefinition::EntityDefinition()
{

}

// DESTRUCTOR
EntityDefinition::~EntityDefinition()
{

}

EntityDefinition::EntityDefinition(XMLElement& xmlElement) 
{
	m_name = ParseXmlAttribute(xmlElement, "name", m_name);
	if(m_name == "Brick")
	{
		m_type = BRICK;
	}
	XMLElement *childElement = xmlElement.FirstChildElement();

	for (; childElement != nullptr; childElement = childElement->NextSiblingElement())
	{
		const char* childValue = childElement->Value();
		std::string childValueStr; childValueStr.append(childValue);

		if (childValueStr == "RGB")
		{
			XMLElement *xmlRgbElement = xmlElement.FirstChildElement("RGB");
			Rgba rgba = ::ParseXmlAttribute(*xmlRgbElement, "color", Rgba::WHITE);
			if (rgba.a != 0) // if alpha = 0 ignoring
			{
				s_RGBDefinition.insert(std::map<std::string, EntityDefinition*>::value_type(rgba.GetAsString(), this));
			}
		}

		if (childValueStr == "SpriteAnimSet")
		{
			XMLElement *xmlSpriteAnimSetElement = xmlElement.FirstChildElement("SpriteAnimSet");
			SpriteAnimSetDefinition *spriteAnimSetDefinition = new SpriteAnimSetDefinition(*xmlSpriteAnimSetElement);
			m_spriteAnimSetDef = spriteAnimSetDefinition;

			XMLElement *xmlSpriteAnimElement = xmlSpriteAnimSetElement->FirstChildElement("SpriteAnim");
			for (; xmlSpriteAnimElement != nullptr; xmlSpriteAnimElement = xmlSpriteAnimElement->NextSiblingElement())
			{
				SpriteAnimationDefinition *spriteAnimationDef = new SpriteAnimationDefinition(*xmlSpriteAnimElement);
				std::string spriteAnimationDefName = spriteAnimationDef->m_name;
				spriteAnimSetDefinition->m_spriteAnimationDefinition.insert(std::map<std::string, SpriteAnimationDefinition*>::value_type	(spriteAnimationDefName, spriteAnimationDef));
			}
		}

		if (childValueStr == "Movement")
		{
			XMLElement *xmlMovementElement = xmlElement.FirstChildElement("Movement");
			m_speed = ParseXmlAttribute(*xmlMovementElement, "speed", m_speed);
			XMLElement *xmlFlyElement = xmlMovementElement->FirstChildElement("Fly");
			if (xmlFlyElement != nullptr)
			{
				m_speed = ParseXmlAttribute(*xmlFlyElement, "speed", m_speed);
			}
		}

		if (childValueStr == "Health")
		{
			XMLElement *xmlHealthElement = xmlElement.FirstChildElement("Health");
			m_healthStart = ParseXmlAttribute(*xmlHealthElement, "start", m_healthStart);
			m_healthMax = ParseXmlAttribute(*xmlHealthElement, "max", m_healthMax);
		}

		if (childValueStr == "Size")
		{
			XMLElement *xmlSizeElement = xmlElement.FirstChildElement("Size");
			m_physicsRadius = ParseXmlAttribute(*xmlSizeElement, "physicsRadius", m_physicsRadius);
			m_drawRadius    = ParseXmlAttribute(*xmlSizeElement, "drawRadius", m_drawRadius);
			m_length		= ParseXmlAttribute(*xmlSizeElement,"length",m_length);
			m_height		= ParseXmlAttribute(*xmlSizeElement,"height",m_height);
		}
		if (childValueStr == "Tags")
		{
			XMLElement *xmlSizeElement = xmlElement.FirstChildElement("Tags");
			std::string tag = "";
			tag = ParseXmlAttribute(*xmlSizeElement, "tag", tag);
		}
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/24
*@purpose : Loads XML and set vvalues to new definition class
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void EntityDefinition::LoadDefinition(std::string xmlPath)
{
	TinyXMLDocument doc;
	doc.LoadFile(xmlPath.c_str());

	XMLElement *xmlRootElement = doc.FirstChildElement("ActorDefinitions");

	XMLElement *xmlChildElement = xmlRootElement->FirstChildElement("ActorDefinition");
	for (; xmlChildElement != nullptr; xmlChildElement = xmlChildElement->NextSiblingElement())
	{
		std::string name = "actor";
		name = ParseXmlAttribute(*xmlChildElement, "name", name);
		EntityDefinition *actorDef = new EntityDefinition(*xmlChildElement);

		s_definitions.insert(std::map<std::string, EntityDefinition*>::value_type(name, actorDef));
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/24
*@purpose : Gets entity def from map
*
*@param   : NIL
*
*@return  : definition
*/
//////////////////////////////////////////////////////////////
EntityDefinition* EntityDefinition::GetDefinition(std::string name)
{
	return s_definitions[name];
}

EntityDefinition* EntityDefinition::getDefinitionByRGB(std::string value)
{
	if (s_RGBDefinition[value] == nullptr)
	{
		return nullptr;
	}
	std::map<std::string, EntityDefinition*>::iterator it;
	it = s_RGBDefinition.find(value);
	return it->second;
}
