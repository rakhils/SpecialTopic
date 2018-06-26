#include <algorithm>
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Core/Windows.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Debug/DebugDraw.hpp"
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Scene::isExitingScene(float deltaTime)
{
	UNUSED(deltaTime);
	if (m_isExitTaskStarted && !m_isExitTaskComplete)
	{
		if (m_exitTime < m_fadingTime)
		{
			return true;
		}
		m_isExitTaskComplete = true;
		return false;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Exits scene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::ExitScene()
{
	m_exitTime = 0;
	m_isExitTaskStarted  = true;
	m_isExitTaskComplete = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::UpdateExitingTime(float deltaTime)
{
	m_exitTime += deltaTime;
	if (m_exitTime > m_fadingTime)
	{
		m_isExitTaskComplete = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Do a full screen black fade in
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::DoFullScreenFadeIn()
{
	float width  = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float height = static_cast<float>(Windows::GetInstance()->GetDimensions().y);
	Rgba  color;
	float colorAplhaValue = RangeMapFloat(m_enterTime, 0, m_fadingTime, 1, 0);
	color.SetAsFloats(1, 0, 0, colorAplhaValue);
	//DebugDraw::GetInstance()->DebugRenderLogf("ENTER COLOR ALPHA : %f", colorAplhaValue);
	Material *material = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(material);
	Renderer::GetInstance()->DrawAABB(AABB2(Vector2(0, 0), Vector2(width, height)), color);
	delete material;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : De a full screen fade out of black color
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::DoFullScreenFadeOut()
{
	float width  = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float height = static_cast<float>(Windows::GetInstance()->GetDimensions().y);
	Rgba  color;
	float colorAplhaValue = RangeMapFloat(m_exitTime, 0, m_fadingTime, 0, 1);
	color.SetAsFloats(0, 1, 0, colorAplhaValue);
	//DebugDraw::GetInstance()->DebugRenderLogf("EXIT COLOR ALPHA : %f", colorAplhaValue);
	Material *material = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(material);
	Renderer::GetInstance()->DrawAABB(AABB2(Vector2(0, 0), Vector2(width, height)), color);
	delete material;
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
			float intensityAtPositionI = CalculateIntensityAtPosition(position, m_lights.at(indexI), m_lights.at(indexI)->GetAttenuationFactor());
			float intensityAtPositionJ = CalculateIntensityAtPosition(position, m_lights.at(indexJ), m_lights.at(indexJ)->GetAttenuationFactor());
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
	Vector3 distanceVector = (posiion - light->GetPosition());
	return light->CalculateIntensityOnAttenutation(distanceVector.GetLength(), attenuationFactor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::Reset()
{
	m_isExitTaskStarted   = false;
	m_isExitTaskComplete  = false;
	m_isEntryTaskStarted  = false;
	m_isEntryTaskComplete = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::UpdateEnteringTime(float deltaTime)
{
	m_enterTime += deltaTime;
	if(m_enterTime > m_fadingTime)
	{
		m_isEntryTaskComplete = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Starts new scene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::EnterScene()
{
	m_isEntryTaskStarted  = true;
	m_isEntryTaskComplete = false;
	m_enterTime = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Trigger when player enters scene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Scene::IsEnteringScene(float deltaTime)
{
	UNUSED(deltaTime);
	if (m_isEntryTaskStarted && !m_isEntryTaskComplete)
	{
		if (m_enterTime < m_fadingTime)
		{
			return true;
		}
		return false;
	}
return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::Update(float deltaTime)
{
	UNUSED(deltaTime);
}
