#pragma once
#include "Engine/Math/Transform.hpp"
/*\class  : Component		   
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 5/17/2018 2:37:11 AM
* \contact: srsrakhil@gmail.com
*/
class GameObject;
class Component
{

public:
	//Member_Variables
	GameObject * m_gameObject = nullptr;
	Transform    m_transform;
	//Static_Member_Variables

	//Methods

	Component();
	~Component();

	virtual void Update(float deltaTime) = 0;
	
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