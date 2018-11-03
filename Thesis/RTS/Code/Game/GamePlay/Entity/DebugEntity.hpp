#pragma once
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Engine/Core/Rgba.hpp"
/*\class  : DebugEntity		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :	   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 10/30/2018 8:47:20 PM
* \contact: srsrakhil@gmail.com
*/
 
class DebugEntity : public Entity
{

public:
	//Member_Variables
	std::string m_debugString;
	float m_fontSize;
	Rgba  color;
	float m_isCompleted;
	float m_startTime;
	//Static_Member_Variables
	//Methods

	DebugEntity();
	~DebugEntity();

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