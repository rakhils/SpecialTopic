#pragma once
#include <vector>

#include "Engine/Math/Vector3.hpp"
/*\class  : RenderScene
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
* \date   : 4/24/2018 5:08:22 PM
*
* \contact: srsrakhil@gmail.com
*/
class ForwardRenderingPath;
class Renderable;
class Light;
class Camera;
class TextureCube;
class Scene
{

public:
	//Member_Variables
	std::string				   m_name;
	float					   m_enterTime  = 0;
	float					   m_fadingTime = 2;
	float					   m_exitTime   = 0;
	bool					   m_isSkyBoxEnabled = false;
	bool					   m_isEntryTaskComplete = false;
	bool					   m_isEntryTaskStarted  = false;
	bool					   m_isExitTaskComplete  = false;
	bool					   m_isExitTaskStarted   = false;
	TextureCube			       *m_texturecube    = nullptr;
	std::vector<Renderable*>   m_renderables;
	std::vector<Light *>       m_lights;
	std::vector<Camera *>      m_cameras;
	ForwardRenderingPath       *m_forwardRenderingPath = nullptr;
	//std::vector<GameObject *>  m_gameObjects;
	//Static_Member_Variables

	//Methods
	Scene();
	Scene(std::string name);
	virtual ~Scene();

	void					AddRenderable(Renderable *renderable);
	void					RemoveRenderable(Renderable *renderable);
	void					AddLight(Light *light);
	void					AddCamera(Camera *camera);
	float					CalculateIntensityAtPosition(Vector3 posiion, Light *light, Vector3 attenuation);

	void					Reset();
	void					UpdateEnteringTime(float deltaTime);
	void					EnterScene();
	bool					IsEnteringScene(float deltaTime);
	virtual void		    Update(float deltaTime);
	bool					isExitingScene(float deltaTime);
	void					ExitScene();
	void					UpdateExitingTime(float deltaTime);

	virtual void			Render() {};
	void					DoFullScreenFadeIn();
	void					DoFullScreenFadeOut();
	std::vector<Light*>		GetMostContributingLights(int lightCount, Vector3 position);
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