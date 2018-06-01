#pragma once
#include "Engine/GameObject/GameObject.hpp"
/*\class  : Asteroid
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
* \date   : 5/4/2018 4:09:06 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class Asteroid: public GameObject
{

public:
	//Member_Variables
	int   m_index = 0;
	float m_radius;
	float m_health;
	//Static_Member_Variables

	//Methods

	Asteroid();
	~Asteroid();
	
	void Update(float deltaTime);
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