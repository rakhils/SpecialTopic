#pragma once
#include <map>
#include <string>

#include "Engine/TinyXml2/tinyxml2.h"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
/*\class  : EntityDefinition
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
* \date   : 2/24/2018 6:04:37 PM
*
* \contact: srsrakhil@gmail.com
*/
 enum Entity_Type
 {
	 BRICK,
	 COIN_BOX,
	 PIPE,
	 POWER_UP,

 };
class EntityDefinition
{

public:
	//Member_Variables
	std::string m_name;
	Entity_Type m_type;
	float m_speed			 = 1.0f;
	int   m_healthMax		 = 1;
	int   m_healthStart      = 1;
	float m_physicsRadius    = 0.4f;
	float m_length			 = 1.f;
	float m_height			 = 1.f;
	float m_drawRadius       = 0.5f;
	SpriteAnimSetDefinition *m_spriteAnimSetDef = nullptr;
	//Static_Member_Variables
	static std::map<std::string, EntityDefinition*> s_definitions;
	static std::map<std::string, EntityDefinition*> s_RGBDefinition;
	//Methods

	EntityDefinition();
	EntityDefinition(XMLElement& element);
	~EntityDefinition();

	//Static_Methods
	static void LoadDefinition(std::string xmlPath);
	static EntityDefinition* GetDefinition(std::string name);
	static EntityDefinition* getDefinitionByRGB(std::string value);
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