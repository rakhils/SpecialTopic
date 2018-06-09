#pragma once
#include "Game/GamePlay/Tiles/TileDefinition.hpp"
//#include "MapDefinition.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/TinyXml2/tinyxml2.h"
//#include "MapGenSteps_FillAndEdge.hpp"

//#include "Map.hpp"
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

class MapGenSteps_FromFile: public MapGenSteps
{
public:
	Image * m_image;
	std::string imagePath = "";
	MapGenSteps_FromFile::MapGenSteps_FromFile(XMLElement& generationStepElement)
		: MapGenSteps(generationStepElement)
	{
		imagePath = ParseXmlAttribute(generationStepElement, "imagepath", imagePath);
		m_image = new Image(imagePath);
	}
	virtual void Run(Map &newMap);
};