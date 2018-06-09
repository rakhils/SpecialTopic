#include "MapDefinition.hpp"

std::map<std::string,MapDefinition*> MapDefinition::s_definition;

MapDefinition::MapDefinition(XMLElement& xml)
{
	
}

void MapDefinition::LoadDefintions(std::string xmlPath)
{
	TinyXMLDocument doc;
	doc.LoadFile(xmlPath.c_str());
	XMLElement *xmlRootElement = doc.FirstChildElement( "MapDefinitions" );
	XMLElement *xmlChildElement = xmlRootElement->FirstChildElement("MapDefinition");
	for(;xmlChildElement !=nullptr;xmlChildElement = xmlChildElement->NextSiblingElement())
	{
		const char* name = xmlChildElement->Attribute("name");
		const char* defaultTile = xmlChildElement->Attribute("defaultTile");
		const char* width = xmlChildElement->Attribute("width");
		const char* height = xmlChildElement->Attribute("height");

		MapDefinition *mapDef = new MapDefinition(*xmlChildElement);
		std::string defaultTileStr; defaultTileStr.append(defaultTile);
		std::string nameStr;nameStr.append(name);
		std::string widthStr;widthStr.append(width);
		std::string heightStr;heightStr.append(height);

		mapDef->m_defaultTile = defaultTileStr;

		IntRange widthRange; widthRange.SetFromText(width);
		IntRange heightRange; heightRange.SetFromText(height);

		mapDef->m_heigth = heightRange.GetRandomInRange();
		mapDef->m_width = widthRange.GetRandomInRange();

		s_definition.insert(std::map<std::string, MapDefinition*>::value_type(nameStr,mapDef));

		XMLElement *xmlGenStepsElement = xmlChildElement->FirstChildElement("GenerationSteps");

		if(xmlGenStepsElement != nullptr)
		{
			XMLElement *xmlGenStepsChildElement = xmlGenStepsElement->FirstChildElement();
			for(;xmlGenStepsChildElement !=nullptr;xmlGenStepsChildElement = xmlGenStepsChildElement->NextSiblingElement())
			{

				const char *rootElement = xmlGenStepsChildElement->Value();
				std::string rootElementStr;	rootElementStr.append(rootElement);
				if(rootElementStr == "FromFile1")
				{
					MapGenSteps *mapGenSteps = MapGenSteps::CreateMapGenStep(*xmlGenStepsChildElement);
					mapDef->m_genSteps.push_back(mapGenSteps);	
				}
				if(rootElementStr == "SpawnActor")
				{
					MapGenSteps *mapGenSteps = MapGenSteps::CreateMapGenStep(*xmlGenStepsChildElement);
					mapDef->m_genSteps.push_back(mapGenSteps);
				}
				if(rootElementStr == "SpawnItem")
				{
					MapGenSteps *mapGenSteps = MapGenSteps::CreateMapGenStep(*xmlGenStepsChildElement);
					mapDef->m_genSteps.push_back(mapGenSteps);
				}
				if(rootElementStr == "RoomsAndPaths")
				{
					MapGenSteps *mapGenSteps = MapGenSteps::CreateMapGenStep(*xmlGenStepsChildElement);
					mapDef->m_genSteps.push_back(mapGenSteps);
				}
			}
		}
	}
}

MapDefinition* MapDefinition::getDefinition(std::string name)
{
	std::map<std::string, MapDefinition*>::iterator it;
	it = s_definition.find(name);
	return it->second;
}
