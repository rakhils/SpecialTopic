#pragma once 
#include "Engine/Math/Matrix44.hpp"
/*\class  : Renderable
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
* \date   : 4/24/2018 5:00:53 PM
*
* \contact: srsrakhil@gmail.com
*/
class Mesh;
class Material;
class Renderable
{

public:
	//Member_Variables
	std::string m_name;
	Matrix44 m_modelMatrix;
	Material *m_material;
	Mesh     *m_mesh;
	int		 m_sortOrder = 0;
	bool	 m_useLight;
	//Static_Member_Variables

	//Methods

	Renderable();
	~Renderable();
	
	void SetMesh(Mesh *mesh);
	void SetMaterial(Material *material);

	Material * GetMaterial() { return m_material; }
	Mesh     * GetMesh()	 { return m_mesh;}
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