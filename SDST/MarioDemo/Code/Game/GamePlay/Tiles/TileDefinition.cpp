#include "Game/GamePlay/TIles/TileDefinition.hpp"
#include "Engine/Core/XMLUtilities.hpp"

std::map<std::string,TileDefinition*> TileDefinition::s_definition;
std::map<std::string,TileDefinition*> TileDefinition::s_RGBDefinition;

TileDefinition::TileDefinition(XMLElement& xml)
{
	
}

void TileDefinition::LoadDefintions(std::string xmlPath)
{
	TinyXMLDocument doc;
	doc.LoadFile(xmlPath.c_str());
	XMLElement *xmlRootElement = doc.FirstChildElement("TileDefinitions");
	XMLElement *xmlChildElement = xmlRootElement->FirstChildElement("TileDefinition");

	for (; xmlChildElement != nullptr; xmlChildElement = xmlChildElement->NextSiblingElement())
	{
		const char* name = xmlChildElement->Attribute("name");
		const char* rgb = xmlChildElement->Attribute("spriteTint");

		TileDefinition *tileDef = new TileDefinition(*xmlChildElement);
		if (rgb != nullptr)
		{
			Rgba rgba = ::ParseXmlAttribute(*xmlChildElement, "spriteTint", Rgba::WHITE);
			if (rgba.a != 0) // if alpha = 0 ignoring
			{
				s_RGBDefinition.insert(std::map<std::string, TileDefinition*>::value_type(rgba.GetAsString(), tileDef));
			}
		}
		std::string nameStr; nameStr.append(name);
		tileDef->m_name = name;
		s_definition.insert(std::map<std::string, TileDefinition*>::value_type(nameStr, tileDef));
	}
}

TileDefinition* TileDefinition::getDefinition(std::string name)
{
	std::map<std::string,TileDefinition*>::iterator it;
	it = s_definition.find(name);
	return it->second;
	
}

TileDefinition* TileDefinition::getRGBDefinition(std::string value)
{
	if(s_RGBDefinition[value] == nullptr)
	{
		return nullptr;
	}
	std::map<std::string, TileDefinition*>::iterator it;
	it = s_RGBDefinition.find(value);
	return it->second;
}

TileDefinition* TileDefinition::ParseXmlAttribute(XMLElement& generationStepElement, std::string name, TileDefinition* m_TileDef )
{
	const char* namechar = generationStepElement.Attribute(name.c_str());
	std::string tilename; tilename.append(namechar);
	TileDefinition *tileDef = (TileDefinition*)s_definition[tilename];
	if(tileDef == nullptr)
	{
		return m_TileDef;
	}

	return s_definition[tilename];
}