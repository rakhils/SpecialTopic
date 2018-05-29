#pragma once
#include <map>
#include "SpriteAnimationDefinition.hpp"
#include "SpriteSheet.hpp"
#include <string>
#include "Engine/Math/IntVector2.hpp"


class SpriteAnimSetDefinition
{
public:

	std::string m_spriteSheetFileName;
	IntVector2 m_spriteLayout;
	std::map<std::string,SpriteAnimationDefinition*> m_spriteAnimationDefinition;
	
	SpriteAnimSetDefinition();
	SpriteAnimSetDefinition(XMLElement& element);
	SpriteAnimationDefinition* getSpriteAnimationDefinition(std::string name);
	
};