#pragma once
#include "Engine/SceneManagement/Scene.hpp"
/*\class  : MainMenu		   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/1/2018 4:17:53 PM
* \contact: srsrakhil@gmail.com
*/
 
class SceneMainMenu : public Scene
{

public:
	//Member_Variables
	int m_cursorIndex = 0;
	bool m_mode = false;
	//Static_Member_Variables

	//Methods

	SceneMainMenu();
	SceneMainMenu(std::string str);
	~SceneMainMenu();
	
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