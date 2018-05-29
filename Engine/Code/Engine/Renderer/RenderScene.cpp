#include "Engine/Renderer/RenderScene.hpp"

// CONSTRUCTOR
RenderScene::RenderScene()
{

}

// DESTRUCTOR
RenderScene::~RenderScene()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/24
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void RenderScene::AddRenderable(Renderable *renderable)
{
	m_renderables.push_back(renderable);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/24
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void RenderScene::AddLight(Light *light)
{
	m_lights.push_back(light);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/24
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void RenderScene::AddCamera(Camera *camera)
{
	m_cameras.push_back(camera);
}
