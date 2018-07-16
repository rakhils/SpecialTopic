#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/GL/glfunctions.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/Renderer/Shaders/ShaderProgramInfo.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
// CONSTRUCTOR
std::map< std::string, ShaderProgram *> ShaderProgram::m_ShaderProgramMap;
ShaderProgram *ShaderProgram::s_currentShaderProgram = nullptr;
ShaderProgram *ShaderProgram::s_defaultShaderProgram = nullptr;

std::string ShaderProgram::vertexShader =			"#version 420 core\n\
													uniform mat4 PROJECTION;\n\
													in vec2 UV;\n\
													in vec4 COLOR;\n\
													in vec3 POSITION;\n\
													out vec2 passUV;\n\
													out vec4 passColor;\n\
													void main(void)\n\
													{\n\
													vec4 local_pos = vec4(POSITION,1);\n\
													vec4 clip_pos = PROJECTION*local_pos;\n\
													passColor = COLOR;\n\
													passUV = UV;\n\
													gl_Position = clip_pos;\n\
													}\n";

std::string ShaderProgram::fragmentShader =			"#version 420 core\n\
													layout(binding = 0) uniform sampler2D gTexDiffuse;\n\
													in vec4 passColor;\n\
													in vec2 passUV;\n\
													out vec4 outColor;\n\
													void main(void)\n\
													{\n\
													vec4 diffuse = texture(gTexDiffuse, passUV);\n\
													outColor = diffuse * passColor;\n\
													}\n";

std::string ShaderProgram::invalidVertexShader   =  "#version 420 core\nin  vec3 POSITION;\nvoid main(void)\n{\ngl_Position = vec4(POSITION, 1.0);\n}\n";
std::string ShaderProgram::invalidFragmentShader =  "#version 420 core\nout vec4 outColor;\nvoid main(void)\n{\noutColor = vec4(1, 0, 1, 1);\n}\n";



ShaderProgram::ShaderProgram(const std::string& filePath)
{
	isShaderLoadedComplete = LoadFromFiles(filePath.c_str());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/28
*@purpose : Default constructor
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
ShaderProgram::ShaderProgram()
{

}

// DESTRUCTOR
ShaderProgram::~ShaderProgram()
{

}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/25
*@purpose : Utility function to load shaders from file
*
*@param   : File name
*
*@return  : Boolean saying Loading success or not success
*/
//////////////////////////////////////////////////////////////
bool ShaderProgram::LoadFromFiles(char const *rootVs,char const *rootFs)
{
	//fileName = root;
	std::string vs_file = rootVs;
	vs_file += ".vs";

	std::string fs_file = rootFs;
	fs_file += ".fs";

	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vert_shader = LoadShaderFromFile(vs_file.c_str(), GL_VERTEX_SHADER);
	GLuint frag_shader = LoadShaderFromFile(fs_file.c_str(), GL_FRAGMENT_SHADER);

	if (vert_shader == NULL || frag_shader == NULL)
	{
		return false;
	}

	// Link the program
	// program_handle is a member GLuint. 
	program_handle = CreateAndLinkProgram(vert_shader, frag_shader);
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return (program_handle != NULL);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/25
*@purpose : Utility function to load shaders from file
*
*@param   : File name
*
*@return  : Boolean saying Loading success or not success
*/
//////////////////////////////////////////////////////////////
bool ShaderProgram::LoadFromFiles(char const *root)
{
	m_fileName = root;
	std::string vs_file = root;
	vs_file += ".vs";

	std::string fs_file = root;
	fs_file += ".fs";

	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vert_shader = LoadShaderFromFile(vs_file.c_str(), GL_VERTEX_SHADER);
	GLuint frag_shader = LoadShaderFromFile(fs_file.c_str(), GL_FRAGMENT_SHADER);

	if(vert_shader == NULL || frag_shader == NULL)
	{
		return false;
	}

	// Link the program
	// program_handle is a member GLuint. 
	program_handle = CreateAndLinkProgram(vert_shader,frag_shader);
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return (program_handle != NULL);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/28
*@purpose : Load default shaderprogram if not loaded once
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool ShaderProgram::LoadFromFiles()
{
	if(m_fileName == "")
	{
		m_fileName = "PASSTHROUGH";
	}
	return LoadFromFiles(m_fileName.c_str());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/28
*@purpose : Loading shaders from strings
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool ShaderProgram::LoadShaders(std::string _vertexShader, std::string _fragmentShader)
{
	GLuint vert_shader = LoadShader((char*)_vertexShader.c_str(), GL_VERTEX_SHADER);
	GLuint frag_shader = LoadShader((char*)_fragmentShader.c_str(), GL_FRAGMENT_SHADER);

	// Link the program
	// program_handle is a member GLuint. 
	program_handle = CreateAndLinkProgram(vert_shader, frag_shader);
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return (program_handle != NULL);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/31
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool ShaderProgram::CreateShadersFrom(char const *rootVs,char const *rootFs,GLenum shaderType,std::string defineSets)
{
	m_fileName = rootVs;
	std::string vs_file = rootVs;
	vs_file += ".vs";

	std::string fs_file = rootFs;
	fs_file += ".fs";

	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	char *vertexShadersrc = (char*)LoadShaderFromFile(vs_file.c_str());
	_ASSERT(vertexShadersrc != nullptr);

	char *fragmentShadersrc = (char*)LoadShaderFromFile(fs_file.c_str());
	_ASSERT(fragmentShadersrc != nullptr);

	std::string vertexShaderStr(vertexShadersrc);
	std::string fragmentShaderStr(fragmentShadersrc);
	
	fragmentShaderStr = CheckForIncludesInShaderProgram(fs_file,fragmentShaderStr);
	
	std::vector<std::string> Strings;
	Split(Strings,defineSets, ';');
	int positionOfFirstLF = static_cast<int>(vertexShaderStr.find_first_of("\n"));// LINE FEED
	positionOfFirstLF++;
	for(int defineSetIndex = 0;defineSetIndex < Strings.size();defineSetIndex++)
	{
		std::string defineSet = Strings.at(defineSetIndex);
		defineSet.insert(0, std::string("#define "));
		defineSet = ReplaceAllChars(defineSet, '=', ' ');
		defineSet.append(";\n");

		if(shaderType == GL_VERTEX_SHADER)
		{
			vertexShaderStr.insert(positionOfFirstLF, defineSet);
		}
		if(shaderType == GL_FRAGMENT_SHADER)
		{
			fragmentShaderStr.insert(positionOfFirstLF, defineSet);
		}
		positionOfFirstLF += static_cast<int>(defineSet.length());
	}

	GLuint vert_shader = LoadShader((char*)vertexShaderStr.c_str(), GL_VERTEX_SHADER);
	//GLuint frag_shader = LoadShader(fragmentShadersrc, GL_FRAGMENT_SHADER);
	GLuint frag_shader = LoadShader((char*)fragmentShaderStr.c_str(), GL_FRAGMENT_SHADER);


	if (vert_shader == NULL || frag_shader == NULL)
	{
		return false;
	}

	// Link the program
	// program_handle is a member GLuint. 
	program_handle = CreateAndLinkProgram(vert_shader, frag_shader);
	GL_CHECK_ERROR();
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return (program_handle != NULL);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/16
*@purpose : Checks for #include string in shaderprogram
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
std::string ShaderProgram::CheckForIncludesInShaderProgram(std::string filefullpathname,std::string shaderProgram)
{
	std::string include = "#include";
	std::string originalStringShaderProgram = shaderProgram;
	std::string oneLine = ReadLine((char*)shaderProgram.c_str());
	int index = 0;
	for(;index < shaderProgram.length() -1;)
	{
		if(StartsWith(oneLine,"#include"))
		{
			std::string fileName = oneLine.substr(include.length() + 1, oneLine.length() - (include.length() - 1));
			if(fileName[0] == '\"')
			{
				fileName = fileName.substr(1, fileName.length() - 3);
			}
			std::string filePath = "";
			int filepathpareseindex = static_cast<int>(filefullpathname.find_last_of("/"));
			if(filepathpareseindex != -1)
			{
				filePath = filefullpathname.substr(0,filepathpareseindex);
			}
			std::string programCode = GetFileContentAsString(filePath.append(fileName).c_str());
			shaderProgram = ReplaceIncludeWithProgram(shaderProgram, programCode, index);
		}

		index += static_cast<int>(oneLine.length() + 1);
		int nextLineEnd = static_cast<int>(shaderProgram.find("\n", index));
		if(nextLineEnd == -1)
		{
			int totalLength = static_cast<int>(shaderProgram.length());
			if(totalLength == index + 1)
			{
				break;
			}
			oneLine = shaderProgram.substr(index, shaderProgram.length() - index - 1);
			break;
		}
		else
		{
			oneLine = shaderProgram.substr(index, nextLineEnd - index);
		}
	}
	return shaderProgram;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/17
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
std::string ShaderProgram::ReplaceIncludeWithProgram(std::string origprogram, std::string includeprogram, int index)
{
	int lineEndIndex = static_cast<int>(origprogram.find("\n", index + 1));
	origprogram.erase(index, lineEndIndex - index + 1);
	origprogram.insert(index, includeprogram);
	return origprogram;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/26
*@purpose : get shader program handle
*
*@param   : NIL
*
*@return  : program handle
*/
//////////////////////////////////////////////////////////////
GLuint ShaderProgram::GetHandle()
{
	return program_handle;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
ShaderProgramInfo * ShaderProgram::GetInfo()
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/22
*@purpose : Creates new or retrieves old shader program
*@param   : file path
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ShaderProgram * ShaderProgram::CreateOrGetShaderProgram(std::string filePath)
{
	if (m_ShaderProgramMap.find(filePath) != m_ShaderProgramMap.end())
	{
		return m_ShaderProgramMap[filePath];
	}
	if (filePath == "default")
	{
		ShaderProgram *shaderProgram = new ShaderProgram();
		shaderProgram->LoadShaders(vertexShader, fragmentShader);
		m_ShaderProgramMap[filePath] = shaderProgram;
		return shaderProgram;
	}
	if (filePath == "invalid")
	{
		ShaderProgram *shaderProgram = new ShaderProgram();
		shaderProgram->LoadShaders(invalidVertexShader, invalidFragmentShader);
		m_ShaderProgramMap[filePath] = shaderProgram;
		return shaderProgram;
	}
	ShaderProgram *shaderProgram = new ShaderProgram();
	//bool isShaderLoadedComplete = shaderProgram->CreateShadersFrom(filePath.c_str(),"vertex","USE_LIGHTING;HAS_BONE_WEIGHTS;MAX_LIGHTS=8;USE_FOG");
	bool success = shaderProgram->LoadFromFiles(filePath.c_str());
	if (!success)
	{
		return CreateOrGetShaderProgram("invalid");
	}
	shaderProgram->isShaderLoadedComplete = true;
	m_ShaderProgramMap[filePath] = shaderProgram;
	return shaderProgram;
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/28
*@purpose : Reload all shader programs without restarting the cpp files
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void ShaderProgram::ReloadAllShaderPrograms()
{
	std::map<std::string, ShaderProgram*>::iterator it = m_ShaderProgramMap.begin();
	for (; it != m_ShaderProgramMap.end(); it++)
	{
		it->second->LoadFromFiles();
	}
}
