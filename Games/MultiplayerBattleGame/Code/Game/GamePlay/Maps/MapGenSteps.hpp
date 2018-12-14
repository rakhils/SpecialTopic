#pragma once
#include "Engine/TinyXml2/tinyxml2.h"

#include "Game/GamePlay/Tiles/TileDefinition.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
using namespace tinyxml2;

class Map;
class MapGenSteps
{
	
public:
	MapGenSteps( const XMLElement& genStepXmlElement );
	MapGenSteps();
	~MapGenSteps();
	//virtual 
	virtual void MapGenSteps::Run( Map &map ) = 0; // "pure virtual", MUST be overridden by subclasses
public:
	static MapGenSteps* CreateMapGenStep(XMLElement& genStepXmlElement );
private:
	std::string m_name;

};
