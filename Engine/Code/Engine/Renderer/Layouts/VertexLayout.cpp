#include "Engine/Renderer/Layouts/VertexLayout.hpp"

// CONSTRUCTOR
VertexLayout::VertexLayout()
{

}

// DESTRUCTOR
VertexLayout::~VertexLayout()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/03
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
int VertexLayout::GetAttributeCount() const
{
	return static_cast<int>(m_attributes.size());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/03
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
VertexAttribute VertexLayout::GetAttribute(int index) const
{
	return m_attributes.at(index);
}
