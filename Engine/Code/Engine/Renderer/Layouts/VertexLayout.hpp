#pragma once
#include <string>
#include <vector>

#include "Engine\Renderer\GL\glfunctions.hpp"
/*\class  : VertexLayout
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
* \date   : 4/2/2018 2:38:46 PM
*
* \contact: srsrakhil@gmail.com
*/
struct VertexAttribute
{
	VertexAttribute(){}
	VertexAttribute(char const *name, GLenum type, int count, bool normalized, size_t offset) 
	{
		m_name = name;
		m_GLType = type;
		m_elementCount = count;
		m_isNormalized = normalized;
		m_memberOffset = offset;
		m_isInitilized = true;
	}

	std::string m_handle; // POSITION

	std::string      m_name = "";
	GLenum           m_GLType;
	int              m_elementCount;
	bool             m_isNormalized;
	bool			 m_isInitilized = false;
	size_t         m_memberOffset; // how far from start of element to this piece of data
};
class VertexLayout
{

public:
	//Member_Variables
	std::vector<VertexAttribute> m_attributes;
	int m_stride; // how far between element
	//Static_Member_Variables

	//Methods
	VertexLayout( size_t stride, VertexAttribute const *layout )
	{
		m_stride =  static_cast<int>(stride);
		while(true)
		{
			if(layout == nullptr || !layout->m_isInitilized)
			{
				break;
			}
			m_attributes.push_back(*layout);
			layout++;
		}
	}
	//void (*CopyVertices)(void *dst, VertexBuilder const &src, int count);

	VertexLayout();
	~VertexLayout();

	int GetAttributeCount() const;
	VertexAttribute GetAttribute(int index) const;
	
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