#pragma once
#include "Engine/Math/Vector2.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
/*\class  : TownCenter		   
* \group  : <GroupName
* \brief  
* \TODO:  : 
* \note   
* \author : Rakhil Soman 
* \version: 1.0
* \date   : 8/21/2018 3:03:56 PM
* \contact: srsrakhil@gmail.com
*/
struct Resources
{
	int m_food = 0;
	int m_wood = 0;
	int m_stone = 0;
};
class TownCenter : public Entity
{

public:
	//Member_Variables
	Resources m_resourceStat;
	//Static_Member_Variables

	//Methods

	TownCenter();
	TownCenter(Map *map, Vector2 position, int teamId);
	~TownCenter();

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