#pragma once
#include <map>

#include "Engine/TinyXml2/tinyxml2.h"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"

using namespace tinyxml2;

class TileDefinition
{

public:

	Vector2 minposition;
	Vector2 maxposition;

	Vector2 m_spriteCords;
	Vector2 m_spriteLayout;

	TileDefinition(XMLElement& xml);
	static void LoadDefintions(std::string xmlPath);
	static TileDefinition* getDefinition(std::string value);
	static std::map<std::string,TileDefinition*> s_definition;
	static TileDefinition* TileDefinition::ParseXmlAttribute(XMLElement& generationStepElement, std::string name, TileDefinition* m_TileDef );

};