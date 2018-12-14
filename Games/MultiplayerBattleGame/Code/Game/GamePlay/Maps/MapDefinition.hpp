#pragma once
#include <map>
#include <string>

#include "Engine/TinyXml2/tinyxml2.h"

#include "Game/GamePlay/Maps/MapGenSteps.hpp"

using namespace tinyxml2;

class MapGenSteps;
class MapDefinition
{
public:
	int m_width;
	int m_heigth;

	MapDefinition(XMLElement& xml);
	std::string m_defaultTile;
	static void LoadDefintions(std::string xmlPath);
	static MapDefinition* getDefinition(std::string name);
	static std::map<std::string,MapDefinition*> s_definition;
	
	std::vector<MapGenSteps*> m_genSteps;
};