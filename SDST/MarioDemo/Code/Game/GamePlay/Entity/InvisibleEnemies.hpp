#pragma once
#include <string>
#include "Game/GamePlay/Entity/Entity.hpp"
/*\class  : InvisibleEnemies
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
* \date   : 7/11/2018 1:08:49 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class InvisibleEnemies : public Entity
{

public:
	//Member_Variables
	float m_radius = 0.0f;
	//Static_Member_Variables

	//Methods

	InvisibleEnemies(std::string name,Vector2 position,float radius);
	~InvisibleEnemies();
	
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