#pragma once
#include <vector>
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

class RenderScene
{

public:
	//Member_Variables
	std::vector<Renderable*> m_renderables;
	std::vector<Light *>     m_lights;
	std::vector<Camera *>    m_cameras;
	//Static_Member_Variables

	//Methods
	RenderScene();
	~RenderScene();
	
	void AddRenderable(Renderable *renderable);
	void AddLight(Light *light);
	void AddCamera(Camera *camera);

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