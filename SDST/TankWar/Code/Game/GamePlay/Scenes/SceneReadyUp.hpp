#pragma once
#include "Engine/SceneManagement/Scene.hpp"
/*\class  : ReadyUp		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/1/2018 4:18:30 PM
* \contact: srsrakhil@gmail.com
*/
 
class SceneReadyUp : public Scene
{

public:
	//Member_Variables
	float m_loadingTime = 0;
	//Static_Member_Variables

	//Methods

	SceneReadyUp();
	SceneReadyUp(std::string str);
	~SceneReadyUp();
	
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