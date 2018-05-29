#include "Engine/SceneManagement/SceneManager.hpp"
#include "Engine/SceneManagement/Scene.hpp"

std::map<std::string, Scene*> SceneManager::s_scenes;

// CONSTRUCTOR
SceneManager::SceneManager()
{

}

// DESTRUCTOR
SceneManager::~SceneManager()
{

	//TODO deletes all scenes in s_scenes
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/29
*@purpose : Creates new scene or retrieves already existing scene
*
*@param   : scene name as id
*
*@return  : scene pointer
*/
//////////////////////////////////////////////////////////////
Scene * SceneManager::CreateOrGetScene(std::string sceneName)
{
	if (s_scenes[sceneName] == nullptr)
	{
		Scene *scene = new Scene(sceneName);
		s_scenes[sceneName] = scene;
		return scene;
	}
	return s_scenes[sceneName];
}
