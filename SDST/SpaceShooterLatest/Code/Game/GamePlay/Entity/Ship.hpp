#pragma once
#include "Engine/GameObject/GameObject.hpp"
/*\class  : Ship
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
* \date   : 5/7/2018 12:25:55 AM
*
* \contact: srsrakhil@gmail.com
*/
 
class Ship : public GameObject
{

public:
	//Member_Variables
	float m_lastBulletSpawnTime;
	float m_bulletSpawnInterval  = 0.25;
	float m_health = 10; 
	bool m_isAlive = true;
	//Static_Member_Variables

	//Methods

	Ship();
	~Ship();
	
	void OnCollisionEnter(Collider* collider);
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