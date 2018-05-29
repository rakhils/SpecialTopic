#include <algorithm>
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
Scene::Scene()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Scene::Scene(std::string name)
{
	m_name = name;
}

// DESTRUCTOR
Scene::~Scene()
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
void Scene::AddRenderable(Renderable *renderable)
{
	m_renderables.push_back(renderable);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : Removes renderable from list
*
*@param   : renderble to be deleted
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Scene::RemoveRenderable(Renderable *renderable)
{

	for(int index = 0;index < m_renderables.size(); index++)
	{
		if(m_renderables.at(index) == renderable)
		{
			/*if(m_renderables.at(index) != nullptr && m_renderables.at(index)->m_mesh != nullptr)
			{
				delete m_renderables.at(index)->m_mesh;
				m_renderables.at(index)->m_mesh = nullptr;
			}
			if(m_renderables.at(index)!= nullptr)
			{
				delete m_renderables.at(index);
				m_renderables.at(index) = nullptr;

			}			*/
			m_renderables.erase(m_renderables.begin() + index);
			index--;
			return;
		}
	}
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
void Scene::AddLight(Light *light)
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
void Scene::AddCamera(Camera *camera)
{
	m_cameras.push_back(camera);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/01
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
std::vector<Light*> Scene::GetMostContributingLights(int lightCount, Vector3 position)
{
	UNUSED(lightCount);
	for(int indexI = 0;indexI < m_lights.size();indexI++)
	{
		for(int indexJ = indexI+1;indexJ < m_lights.size();indexJ++)
		{
			float intensityAtPositionI = CalculateIntensityAtPosition(position, m_lights.at(indexI), m_lights.at(indexI)->GetPointLightAttenuationFactor());
			float intensityAtPositionJ = CalculateIntensityAtPosition(position, m_lights.at(indexJ), m_lights.at(indexJ)->GetPointLightAttenuationFactor());
			if(intensityAtPositionJ > intensityAtPositionI)
			{
				Light *temp = m_lights.at(indexJ);
				m_lights.at(indexJ) = m_lights.at(indexI);
				m_lights.at(indexI) = temp;
			}
		}
	}
	return m_lights;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/02
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
float Scene::CalculateIntensityAtPosition(Vector3 posiion, Light *light, Vector3 attenuationFactor)
{
	Vector3 distanceVector = (posiion - light->GetPointLightPosition());
	return light->CalculateIntensityOnAttenutation(distanceVector.GetLength(), attenuationFactor);
}
