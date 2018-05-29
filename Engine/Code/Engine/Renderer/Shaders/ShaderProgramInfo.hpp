#pragma once
#include <map>
/*\class  : ShaderProgramInfo
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
* \date   : 4/29/2018 2:12:10 AM
*
* \contact: srsrakhil@gmail.com
*/
class PropertyBlockInfo;
class PropertyInfo;
class ShaderProgramInfo
{

public:
	//Member_Variables
	std::map<std::string,PropertyInfo*> m_blockInfos; 
	//Static_Member_Variables

	//Methods

	ShaderProgramInfo();
	~ShaderProgramInfo();
	
	PropertyBlockInfo* FindBlockInfo(char const *block_name);
	PropertyBlockInfo* FindContainingBlock(char const *prop_name);
	PropertyInfo*      GetPropertyInfo(char const *name);
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