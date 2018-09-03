#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
/*\class  : Resource		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/21/2018 9:30:35 PM
* \contact: srsrakhil@gmail.com
*/
class Map;
class Resource : public Entity
{

public:
	//Member_Variables
	//Static_Member_Variables

	//Methods

	Resource();
	Resource(Map *map,Vector2 position,EntityType type);
	~Resource();

	void ProcessInputs(float deltaTime);
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