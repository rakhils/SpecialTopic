#include "SpriteAnimationDefinition.hpp"

SpriteAnimationDefinition::SpriteAnimationDefinition()
{

}

SpriteAnimationDefinition::SpriteAnimationDefinition(XMLElement& element)
{
	m_name = ParseXmlAttribute(element,"name",m_name);
	m_spriteIndexes = ParseXmlAttribute(element,"spriteIndexes",m_spriteIndexes);
}

SpriteAnimationDefinition::SpriteAnimationDefinition(const XMLElement& animElement, float defaultFps)
{
	m_name = ParseXmlAttribute(animElement,"name",m_name);
	m_spriteIndexes = ParseXmlAttribute(animElement,"spriteIndexes",m_spriteIndexes);
	m_framesPerSecond = defaultFps;
}

