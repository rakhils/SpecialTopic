#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/TinyXml2/tinyxml2.h"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Sprites.hpp"

/*\class  : IsoSprites
*		   
* \group  : <GroupName>
*		   
* \brief  :
*		   
* \TODO:  :
*		   
* \note   :
*		   
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 3/6/2018 4:11:31 PM
*
* \contact: srsrakhil@gmail.com
*/

using namespace tinyxml2;
class Renderer;
struct IsoSpriteFacingDirection
{
	std::string m_srcSpriteName;
	IntVector2  m_scale;
};
class IsoSpriteDefinition
{
	/*<isosprite id = "archer_f.walk0">
		<facing dir = "-1, 1" src = "archer_f.al.walk0" / >
		<facing dir = " 1, 1" src = "archer_f.al.walk0" scale = "-1, 1" / >
		<facing dir = "-1,-1" src = "archer_f.tr.walk0" scale = "-1, 1" / >
		<facing dir = " 1,-1" src = "archer_f.tr.walk0" / >
		< / isosprite>*/
public:

	std::string m_id;
	SpriteDefinition m_spriteDef;
	std::map<std::string, IsoSpriteFacingDirection*> m_isoSpriteFacingDirections;
	IsoSpriteDefinition();
	IsoSpriteDefinition(XMLElement& element);

};

class IsoSprites
{

public:
	//Member_Variables
	Sprites			    *m_currentSprite		  = nullptr;
	std::string		     m_spriteId;
	IsoSpriteDefinition *m_definition;
	Renderer			*renderer				  = nullptr;
	//Static_Member_Variables

	static std::map<std::string, IsoSpriteDefinition*> s_isoSpriteDefinition;
	//Methods

	IsoSprites(){}
	IsoSprites(IsoSpriteDefinition *def);
	~IsoSprites();
	

	AABB2 GetTexCoordsForSpriteIndex();
	std::string GetDirectionAsString(Vector2 direction);
	Sprites* GetSpriteWithDirection(Vector2 direction);
	IntVector2 GetScaleFactor(Vector2 direction);
	static void LoadDefinition(std::string filePathName);
	static IsoSpriteDefinition* GetDefinition(std::string name);
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};