#include "Engine/Renderer/IsoSprites.hpp"
#include "Engine/Renderer/Renderer.hpp"

std::map<std::string, IsoSpriteDefinition*> IsoSprites::s_isoSpriteDefinition;

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/11
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
IsoSpriteDefinition::IsoSpriteDefinition(XMLElement& element)
{
	/*  <isosprite id="archer_f.walk0">
   <facing dir="-1, 1" src="archer_f.al.walk0" />
   <facing dir=" 1, 1" src="archer_f.al.walk0" scale="-1, 1" />
   <facing dir="-1,-1" src="archer_f.tr.walk0" scale="-1, 1" />
   <facing dir=" 1,-1" src="archer_f.tr.walk0" />
	</isosprite>
	*/
	m_id = ParseXmlAttribute(element, "id", m_id);
	XMLElement *childElement = element.FirstChildElement();
	for (; childElement != nullptr; childElement = childElement->NextSiblingElement())
	{
		const char* childValue = childElement->Value();
		std::string childValueStr; childValueStr.append(childValue);
		if(childValueStr == "facing")
		{
			IsoSpriteFacingDirection *facingDirection = new IsoSpriteFacingDirection();
			std::string direction = "";
			direction = ParseXmlAttribute(*childElement, "dir", direction);
			facingDirection->m_srcSpriteName = ParseXmlAttribute(*childElement, "src", facingDirection->m_srcSpriteName);
			facingDirection->m_scale         = ParseXmlAttribute(*childElement, "scale", IntVector2(1,1));
			m_isoSpriteFacingDirections[direction] = facingDirection;
		}
	}
}



//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/20
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
IsoSpriteDefinition::IsoSpriteDefinition()
{

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
IsoSprites::IsoSprites(IsoSpriteDefinition *def)
{
	m_definition = def;
	m_currentSprite = Sprites::CreateOrGetSprites(m_definition->m_isoSpriteFacingDirections[" 1, 1"]->m_srcSpriteName);
}

// DESTRUCTOR
IsoSprites::~IsoSprites()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/20
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
AABB2 IsoSprites::GetTexCoordsForSpriteIndex()
{
	return m_currentSprite->GetTexCoordsForSpriteIndex();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/20
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
std::string IsoSprites::GetDirectionAsString(Vector2 direction)
{
	int xValue = static_cast<int>(direction.x);
	int yValue = static_cast<int>(direction.y);
	std::string returnValue;
	if(xValue == 1)
	{
		returnValue.append(" 1");
	}
	else
	{
		returnValue.append(std::to_string(xValue));
	}
	returnValue.append(",");
	if (yValue == 1)
	{
		returnValue.append(" 1");
	}
	else
	{
		returnValue.append(std::to_string(yValue));
	}
	return returnValue;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/20
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Sprites* IsoSprites::GetSpriteWithDirection(Vector2 direction)
{

	std::string directionString = GetDirectionAsString(direction);
	// tr or al
	std::string spriteId = m_definition->m_isoSpriteFacingDirections[directionString]->m_srcSpriteName;
	
	if (m_currentSprite == nullptr)
	{
		m_currentSprite = new Sprites(Sprites::GetDefinition(spriteId));
	}

	if (m_currentSprite != nullptr)
	{
		m_currentSprite->m_definition = Sprites::GetDefinition(spriteId);
	}
	return m_currentSprite;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/21
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
IntVector2 IsoSprites::GetScaleFactor(Vector2 direction)
{
	std::string directionString = GetDirectionAsString(direction);
	return m_definition->m_isoSpriteFacingDirections[directionString]->m_scale;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/11
*@purpose : Loads and stores all definitions
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void IsoSprites::LoadDefinition(std::string xmlPath)
{
	/* <isosprite id="archer_f.walk0">
   <facing dir="-1, 1" src="archer_f.al.walk0" />
   <facing dir=" 1, 1" src="archer_f.al.walk0" scale="-1, 1" />
   <facing dir="-1,-1" src="archer_f.tr.walk0" scale="-1, 1" />
   <facing dir=" 1,-1" src="archer_f.tr.walk0" />
</isosprite>
	*/
	TinyXMLDocument doc;
	doc.LoadFile(xmlPath.c_str());
	XMLElement *xmlRootElement  = doc.FirstChildElement("IsoSprites");
	XMLElement *xmlChildElement = xmlRootElement->FirstChildElement("isosprite");
	for (; xmlChildElement != nullptr; xmlChildElement = xmlChildElement->NextSiblingElement())
	{
		IsoSpriteDefinition *isoDef = new IsoSpriteDefinition(*xmlChildElement);
		s_isoSpriteDefinition[isoDef->m_id] = isoDef;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/12
*@purpose : Retuns definitions
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
IsoSpriteDefinition* IsoSprites::GetDefinition(std::string name)
{
	return s_isoSpriteDefinition[name];
}

