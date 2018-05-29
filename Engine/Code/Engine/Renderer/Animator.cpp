#include "Engine/Renderer/Animator.hpp"
#include "Engine/Renderer/Renderer.hpp"

std::map<std::string, AnimatorDefinition*> Animator::s_animators;
//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
AnimatorDefinition::AnimatorDefinition(XMLElement &element)
{
	m_id		     = ParseXmlAttribute(element, "id", m_id);
	m_loopTypeString = ParseXmlAttribute(element, "loop", m_loopTypeString);

	XMLElement *childElement = element.FirstChildElement();
	for (; childElement != nullptr; childElement = childElement->NextSiblingElement())
	{
		const char* childValue = childElement->Value();
		std::string childValueStr; childValueStr.append(childValue);
		if (childValueStr == "frame")
		{
			Animation *animation = new Animation();

			animation->m_isoSpriteId = ParseXmlAttribute(*childElement, "src",  animation->m_isoSpriteId);
			animation->m_duration    = ParseXmlAttribute(*childElement, "time", animation->m_duration);
			m_animation.push_back(animation);
		}
	}
}
// CONSTRUCTOR
//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Animator::Animator(AnimatorDefinition *def)
{
	m_definition = def;
	m_animationStartTime = 0.0f;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Animator::Animator()
{

}

// DESTRUCTOR
Animator::~Animator()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
IsoSprites* Animator::GetCurrentSprite()
{
	return m_currentIsoSprite;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : Gets sprite with given direction change
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
IsoSprites* Animator::GetSpriteWithDirection(Vector2 direction)
{
	std::string spriteName = m_definition->m_animation.at(0)->m_isoSpriteId;
	float time = 0;
	for (int spriteIndex = 0; spriteIndex < m_definition->m_animation.size(); spriteIndex++)
	{
		time += m_definition->m_animation.at(spriteIndex)->m_duration;
		if (time > m_currentTime)
		{
			spriteName = m_definition->m_animation.at(spriteIndex)->m_isoSpriteId;
			break;
		}
	}
	
	if(m_currentIsoSprite == nullptr)
	{
		m_currentIsoSprite = new IsoSprites(IsoSprites::GetDefinition(spriteName));
	}

	if(m_currentIsoSprite!= nullptr)
	{
		m_currentIsoSprite->m_definition = IsoSprites::GetDefinition(spriteName);
		m_currentIsoSprite->m_currentSprite = Sprites::CreateOrGetSprites(m_currentIsoSprite->m_definition->m_isoSpriteFacingDirections[" 1, 1"]->m_srcSpriteName);
	}
	return m_currentIsoSprite;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Animator::Update(float deltaTime)
{
	if(m_animationStartTime == 0.0f)
	{
		m_animationStartTime = static_cast<float>(GetCurrentTimeSeconds());
	}
	m_currentTime += deltaTime;
	float time = 0;
	for (int spriteIndex = 0; spriteIndex < m_definition->m_animation.size(); spriteIndex++)
	{
		time += m_definition->m_animation.at(spriteIndex)->m_duration;
	}
	if(m_currentTime > time)
	{
		isComplete = true;
		m_currentTime = 0.0f;
		m_animationStartTime = static_cast<float>(GetCurrentTimeSeconds());
	}
	else
	{
		isComplete = false;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/17
*@purpose : returns is complete bool
*
*@param   : NIL
*
*@return  : bool
*/
//////////////////////////////////////////////////////////////
bool Animator::IsComplete()
{
	return isComplete;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : Loads definitions
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Animator::LoadDefinition(std::string xmlPath)
{
	TinyXMLDocument doc;
	doc.LoadFile(xmlPath.c_str());
	XMLElement *xmlRootElement = doc.FirstChildElement("IsoAnimationDefinitions");
	XMLElement *xmlChildElement = xmlRootElement->FirstChildElement("IsoAnimation");
	for (; xmlChildElement != nullptr; xmlChildElement = xmlChildElement->NextSiblingElement())
	{
		AnimatorDefinition *animatorDef = new AnimatorDefinition(*xmlChildElement);
		
		s_animators[animatorDef->m_id] = animatorDef;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : Returns Animatordefinition by name
*
*@param   : NIL
*
*@return  : 
*/
//////////////////////////////////////////////////////////////
AnimatorDefinition* Animator::GetDefinition(std::string idname)
{
	std::map<std::string , AnimatorDefinition*>::iterator it;
	it = s_animators.find(idname);
	return it->second;
}

