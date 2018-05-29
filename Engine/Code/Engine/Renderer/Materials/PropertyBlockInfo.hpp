#pragma once
#include <string>
#include <vector>
#include "Engine/Renderer/UniformBuffer.hpp"
/*\class  : PropertyBlockInfo
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
* \date   : 4/29/2018 2:04:42 AM
*
* \contact: srsrakhil@gmail.com
*/
class PropertyBlockInfo;
class PropertyBlock : public UniformBuffer
{
public:
	PropertyBlockInfo *m_info;
};

class PropertyInfo
{
public:
	PropertyBlockInfo * m_owningBlock;

	std::string m_name;
	size_t	    m_offset;
	size_t		m_size;
	size_t		m_count;
};

class PropertyBlockInfo
{

public:
	//Member_Variabless
	std::string m_name;
	std::vector<PropertyInfo*> m_infos;
	size_t m_maxSize;
	//Static_Member_Variables

	//Methods

	PropertyBlockInfo();
	~PropertyBlockInfo();
	
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