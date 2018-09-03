#pragma once
#include "Game/GamePlay/Entity/Entity.hpp"
/*\class  : Civilian	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/18/2018 11:21:22 PM
* \contact: srsrakhil@gmail.com
*/
class Map;
class Civilian : public Entity
{

public:
	//Member_Variables
	Entity * m_resourceType = nullptr;
	//Static_Member_Variables

	//Methods

	Civilian();
	Civilian(Map *map,Vector2 position,int teamID);
	~Civilian();

	void ProcessInputs(float deltaTime);
	void Update(float deltaTime);
	void Render();

	void GatherResource(Vector2 resourcePosition);
	
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