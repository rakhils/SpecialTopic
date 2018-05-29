#pragma once
#include <vector>
#include <string>

#include "Engine/TinyXml2/tinyxml2.h"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

using namespace tinyxml2;

class SpriteAnimationDefinition
{
public:

	std::vector<int> m_spriteIndexes;
	std::string m_name;
	float m_duration = 1;
	float m_rate = 1;
	float m_framesPerSecond = 0;

	SpriteAnimationDefinition();
	SpriteAnimationDefinition(XMLElement &element);
	SpriteAnimationDefinition(const XMLElement& animElement,float defaultFps);

	float GetDuration() const { return (float) m_spriteIndexes.size() / m_framesPerSecond; }
};