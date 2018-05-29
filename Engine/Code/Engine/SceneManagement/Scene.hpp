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
class Renderable;
class Light;
class Camera;

class Scene
{

public:
	//Member_Variables
	std::string				 m_name;
	std::vector<Renderable*> m_renderables;
	std::vector<Light *>     m_lights;
	std::vector<Camera *>    m_cameras;
	//Static_Member_Variables

	//Methods
	Scene();
	Scene(std::string name);
	~Scene();

	void					AddRenderable(Renderable *renderable);
	void					RemoveRenderable(Renderable *renderable);
	void					AddLight(Light *light);
	void					AddCamera(Camera *camera);
	float					CalculateIntensityAtPosition(Vector3 posiion, Light *light, Vector3 attenuation);

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