#include "SpriteAnimSetDefinition.hpp"

SpriteAnimSetDefinition::SpriteAnimSetDefinition()
{
	m_spriteSheetFileName = "";
	m_spriteLayout = IntVector2(0,0);
}

SpriteAnimSetDefinition::SpriteAnimSetDefinition(XMLElement& element)
{
	m_spriteSheetFileName = ParseXmlAttribute(element,"spriteSheet",m_spriteSheetFileName);
	m_spriteLayout		  = ParseXmlAttribute(element,"spriteLayout",m_spriteLayout);
}

SpriteAnimationDefinition* SpriteAnimSetDefinition::getSpriteAnimationDefinition(std::string name)
{
	std::map<std::string,SpriteAnimationDefinition*>::iterator it;
	it = m_spriteAnimationDefinition.find(name);
	return it->second;
}

