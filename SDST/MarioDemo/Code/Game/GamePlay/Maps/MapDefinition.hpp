#pragma once
#include <map>
#include "Engine/TinyXml2/tinyxml2.h"
#include <string>
#include "Engine/Math/IntRange.hpp"

#include "MapGenSteps.hpp"
//#include "Game/GamePlay/Map.hpp"
using namespace tinyxml2;

class MapGenSteps;
//class Map;
class MapDefinition
{
public:
	int m_width;
	int m_heigth;
	//MapDefinition();
	MapDefinition(XMLElement& xml);
	std::string m_defaultTile;
	static void LoadDefintions(std::string xmlPath);
	static MapDefinition* getDefinition(std::string name);
	static std::map<std::string,MapDefinition*> s_definition;
	
	std::vector<MapGenSteps*> m_genSteps;
};