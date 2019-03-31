#pragma once
#include "Engine/Math/Vector2.hpp"
/*\class  : Player		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/24/2019 7:39:17 PM
* \contact: srsrakhil@gmail.com
*/
 
class Player
{

public:
	//Member_Variables
	Vector2 m_position;
	float   m_speed = 200;
	int		m_health = 10;
	float   m_radius = 0;
	bool	m_isDead = false;
	//Static_Member_Variables

	//Methods

	Player();
	~Player();

	void Update(float deltaTime);
	void Render();
	
	void Attack();
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