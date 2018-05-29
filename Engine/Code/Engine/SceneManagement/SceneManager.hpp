#pragma once
#include <map>
/*\class  : SceneManager
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
* \date   : 4/29/2018 5:03:07 PM
*
* \contact: srsrakhil@gmail.com
*/
class Scene;
class SceneManager
{

public:
	//Member_Variables
	
	//Static_Member_Variables
	static std::map<std::string, Scene*> s_scenes;
	//Methods

	SceneManager();
	~SceneManager();
	
	//Static_Methods
	static Scene* CreateOrGetScene(std::string sceneName);
	static Scene* GetSceneByName(std::string sceneName);
	static Scene* GetActiveScene();
	static void   LoadScene(std::string sceneName);
	static void   MoveGameObjectToScene(std::string sceneTo);
	static void   SetAsActiveScene(std::string scene);

	

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