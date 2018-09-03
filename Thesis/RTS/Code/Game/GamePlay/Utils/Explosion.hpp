#pragma once
#include "Engine/Math/Vector2.hpp"
/*\class  : Explosion		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 9/1/2018 9:06:55 PM
* \contact: srsrakhil@gmail.com
*/
 
class Explosion
{

public:
	//Member_Variables
	float m_currentRadius = 0;
	float m_direction	  = 1;
	bool  m_completed	  = false;

	Vector2 m_position;
	//Static_Member_Variables

	//Methods

	Explosion(Vector2 position);
	~Explosion();

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