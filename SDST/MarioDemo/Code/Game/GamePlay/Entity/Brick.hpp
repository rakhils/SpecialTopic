#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GameCommon.hpp"
/*\class  : Brick
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
* \date   : 3/1/2018 1:51:03 AM
*
* \contact: srsrakhil@gmail.com
*/
 
class Brick : public Entity
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	Brick();
	Brick(EntityDefinition *definition);
	~Brick();

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