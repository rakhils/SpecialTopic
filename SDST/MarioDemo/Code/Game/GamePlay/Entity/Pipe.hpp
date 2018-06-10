#pragma once
#include <string>
#include "Game/GamePlay/Entity/Entity.hpp"
/*\class  : Pipe
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
* \date   : 3/1/2018 1:58:22 AM
*
* \contact: srsrakhil@gmail.com
*/
 
class Pipe : public Entity
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	
	Pipe(std::string name);
	Pipe(std::string name, EntityDefinition *definition);
	~Pipe();

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