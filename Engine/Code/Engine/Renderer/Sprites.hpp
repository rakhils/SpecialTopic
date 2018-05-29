#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/TinyXml2/tinyxml2.h"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Renderer/Renderer.hpp"

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

class SpriteDefinition
{
	/*  <SpriteDefinition id="archer_f.portrait">
	<diffuse src="image/sprite/to_archer.split.png" />
	<ppu count="16" />
	<uv layout="pixel" uvs="152,13,235,130" />
	<pivot xy="0.5,0" />
	</SpriteDefinition>
	*/
public:

	std::string m_id;
	std::string m_srcImage;
	int m_ppu;
	IntVector2  m_startUV;
	IntVector2  m_endUV;
	Vector2     m_pivotPoint;
	SpriteDefinition();
	SpriteDefinition(XMLElement& element);
	AABB2 GetTexCoordsForSpriteIndex();

};

class Sprites
{

public:
	//Member_Variables
	Texture			*m_texture = nullptr;
	IntVector2		m_isoSpriteLayout;
	std::string     m_isoSpriteSheetFileName;
	std::string     m_isoSpriteName;
	SpriteDefinition *m_definition;
	//Static_Member_Variables
	static std::map< std::string, Sprites *> m_spritesMap;
	//Methods

	Sprites(){}
	Sprites(SpriteDefinition *def);
	~Sprites();

	static std::map<std::string, SpriteDefinition*> s_spriteDefinition;
	
	AABB2 GetTexCoordsForSpriteIndex();
	AABB2 GetTexCoordsForSpriteIndex(IntVector2 minUV,IntVector2 maxUV);
	static void LoadDefinition(std::string filePathName);
	static SpriteDefinition* GetDefinition(std::string name);
	static Sprites         * CreateOrGetSprites(const std::string& spriteID);
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