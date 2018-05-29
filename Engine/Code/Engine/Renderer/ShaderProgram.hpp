#pragma once
/*\class  : ShaderProgram		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 2.0		   
* \date   : 1/25/2018 7:40:10 PM
* \contact: srsrakhil@gmail.com
*/

#include <string>
#include <stdio.h>
#include "cstdlib"
 
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Materials/PropertyBlockInfo.hpp"

class ShaderProgramInfo;
class ShaderProgram
{

public:
	//Member_Variables
	GLuint program_handle = NULL;
	std::string m_fileName = "";
	bool isShaderLoadedComplete = false;
	//Static_Member_Variables
	static std::map< std::string, ShaderProgram *> m_ShaderProgramMap;
	static std::string vertexShader;
	static std::string fragmentShader;
	static std::string invalidVertexShader;
	static std::string invalidFragmentShader;

	static ShaderProgram *s_currentShaderProgram;
	static ShaderProgram *s_defaultShaderProgram;

	//Methods
	ShaderProgram();
	ShaderProgram(const std::string& filePath);
	~ShaderProgram();

	bool LoadFromFiles(char const *rootVs,char const *rootFs);
	bool LoadFromFiles(char const *root);
	bool LoadFromFiles();
	bool LoadShaders(std::string vertexShader,std::string fragmentShader);
	bool CreateShadersFrom(char const *rootVs,char const *rootFs,GLenum shaderType,std::string defineSets);
	std::string CheckForIncludesInShaderProgram(std::string filefullpathname,std::string shaderProgram);
	std::string ReplaceIncludeWithProgram(std::string origprogram,std::string includeprogram,int index);
	GLuint GetHandle(); 
	ShaderProgramInfo *GetInfo();
	//Static_Methods
	static ShaderProgram* CreateOrGetShaderProgram(std::string filePath);

	static ShaderProgram* GetCurrentShaderProgram() { return s_currentShaderProgram;}
	static ShaderProgram* GetDefaultShaderProgram() { return s_defaultShaderProgram;}

	static void			  SetCurrentShaderProgram(ShaderProgram *program) { s_currentShaderProgram = program;}
	static void			  SetDefaultShaderProgram(ShaderProgram *program) { s_defaultShaderProgram = program;}
	static void			  ReloadAllShaderPrograms();
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