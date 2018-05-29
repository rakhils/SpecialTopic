#pragma once
#include <map>
#include <vector>
#include "Engine/TinyXml2/tinyxml2.h"

/*\class  : ShaderDefinitions
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
* \date   : 4/9/2018 4:01:38 PM
*
* \contact: srsrakhil@gmail.com
*/
using namespace tinyxml2;
class Shader;
class Renderer;
class ShaderDefinitions
{

public:
	//Member_Variables
	std::string m_name;
	std::string m_defines;
	std::string m_vertexShaderFileName;
	std::string m_fragmentShaderFileName;

	std::string m_colorOperation;
	std::string m_colorSrc;
	std::string m_colorDst;

	std::string m_alphaOperation;
	std::string m_alphaSrc;
	std::string m_alphaDst;

	std::string m_cull_mode;
	std::string m_depth_compare;
	std::string m_fill_mode;
	std::string m_front_face;

	bool m_depthWrite = false;

	//Static_Member_Variables
	static std::map<std::string, Shader*> s_shaders;
	
	Shader* GetShaderFromDefinition(Renderer *renderer);
	void    SetShaderInitValues(Shader* shader);
	//Methods

	ShaderDefinitions(XMLElement& xml);
	~ShaderDefinitions();
	
	//Static_Methods
	static void LoadDefintions(std::string xmlPath,Renderer *renderer);
	static Shader* GetShaderByDefinitionName(std::string name);
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