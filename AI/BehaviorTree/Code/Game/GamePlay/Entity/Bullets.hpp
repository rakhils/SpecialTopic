#pragma once
#include "Engine/Math/MathUtil.hpp"
/*\class  : Bullets	   
* \group  : <GroupName>	   
* \brief  :	   
* \TODO:  :	   
* \note   :	   
* \author : Rakhil Soman	   
* \version: 1.0	   
* \date   : 3/27/2019 9:48:44 PM
* \contact: srsrakhil@gmail.com
*/
 
class Bullets
{

public:
	//Member_Variables
	Vector2 m_position;
	Vector2 m_direction;
	float   m_radius = 5;
	float   m_speed  = 125;
	float   m_startTime;
	bool	m_isDead = false;
	//Static_Member_Variables

	//Methods

	Bullets();
	~Bullets();
	
	void Update(float deltaTime);


	void Render();
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