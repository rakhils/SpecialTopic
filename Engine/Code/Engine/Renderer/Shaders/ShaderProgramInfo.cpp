#include "Engine/Renderer/Shaders/ShaderProgramInfo.hpp"
#include "Engine/Renderer/Materials/PropertyBlockInfo.hpp"
// CONSTRUCTOR
ShaderProgramInfo::ShaderProgramInfo()
{

}

// DESTRUCTOR
ShaderProgramInfo::~ShaderProgramInfo()
{

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
PropertyInfo* ShaderProgramInfo::GetPropertyInfo(char const *name)
{
	return m_blockInfos[std::string(name)];
}
