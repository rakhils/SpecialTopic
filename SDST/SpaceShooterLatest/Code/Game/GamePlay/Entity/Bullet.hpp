#pragma once
#include "Engine/GameObject/GameObject.hpp"
#include "Game/GameCommon.hpp"
/*\class  : Bullet
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
* \date   : 5/4/2018 4:06:31 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class Bullet : public GameObject
{

public:
	//Member_Variables
	int m_index = 0;
	float m_lifeTime = 6;
	Light *m_light = nullptr;
	//Static_Member_Variables

	//Methods

	Bullet();
	Bullet(Vector3 position, Vector3 direction);
	~Bullet();
	
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