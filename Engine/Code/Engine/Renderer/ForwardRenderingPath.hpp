#pragma once
#include <vector>
/*\class  : ForwardRenderingPath
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
* \date   : 4/25/2018 4:54:12 PM
*
* \contact: srsrakhil@gmail.com
*/
class Scene;
class Camera;
class Renderer;
class TextureCube;
class ForwardRenderingPath
{

public:
	//Member_Variables
	Renderer * m_renderer;
	//Static_Member_Variables

	//Methods

	ForwardRenderingPath();
	ForwardRenderingPath(Renderer *renderer);
	~ForwardRenderingPath();
	
	void RenderScene(Scene *scene);
	void RenderSceneForCamera(Camera *camera, Scene *scene);
	void UpdateLigthBuffer(Scene *scene, int lightCount);
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