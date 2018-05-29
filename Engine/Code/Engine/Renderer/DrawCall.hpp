#pragma once
#include "Engine/Math/Matrix44.hpp"
/*\class  : DrawCall
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
* \date   : 4/30/2018 1:32:58 PM
*
* \contact: srsrakhil@gmail.com
*/
class Mesh;
class Material;
class DrawCall
{

public:
	//Member_Variables
	std::string m_name;
	Mesh     *m_mesh;
	Material *m_material;
	Matrix44 m_modelMatrix;
	int      m_sortOrder = 0;
	//Static_Member_Variables

	//Methods

	DrawCall();
	~DrawCall();
	
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