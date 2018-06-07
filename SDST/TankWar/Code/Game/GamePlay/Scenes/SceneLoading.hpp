#pragma once
#include "Engine/SceneManagement/Scene.hpp"
/*\class  : Loading		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0
* \date   : 6/1/2018 4:17:10 PM
* \contact: srsrakhil@gmail.com
*/
 
class SceneLoading : public Scene
{

public:
	//Member_Variables
	float m_loadingTime = 0;
	Scene *m_level1Scene = nullptr;
	//Static_Member_Variables

	//Methods

	SceneLoading(std::string str);
	~SceneLoading();
	
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