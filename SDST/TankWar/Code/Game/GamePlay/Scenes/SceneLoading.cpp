#include "Engine/Core/Windows.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Logger/LogManager.hpp"
#include "Engine/Debug/DebugDraw.hpp"

#include "Game/GamePlay/Scenes/SceneLevel1.hpp"
#include "Game/GamePlay/Scenes/SceneLoading.hpp"
#include "Game/GameCommon.hpp"
// CONSTRUCTOR

SceneLoading::SceneLoading(std::string str) :Scene(str)
{
/*
	m_forwardRenderingPath = new ForwardRenderingPath();
	AddCamera(Camera::GetUICamera());
	Renderable *textRenderable = new Renderable();
	textRenderable->m_name = "LOADING..";
	textRenderable->m_modelMatrix.SetIdentity();
	float width  = Windows::GetInstance()->GetDimensions().x;
	float height = Windows::GetInstance()->GetDimensions().y;
	textRenderable->m_modelMatrix.Translate3D(Vector3(width/2.0, height/2.0, 0.f));
	textRenderable->m_sortOrder = 1;
	textRenderable->SetMaterial(Material::CreateOrGetMaterial("default"));
	AddRenderable(textRenderable);*/
	Camera::SetCurrentCamera(Camera::GetUICamera());
}

// DESTRUCTOR
SceneLoading::~SceneLoading()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Updates loading scene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLoading::Update(float deltaTime)
{
	
	if (IsEnteringScene(deltaTime))
	{
		UpdateEnteringTime(deltaTime);
	}
	if(m_isEntryTaskComplete)
	{
		m_loadingTime += deltaTime;
	}
	if(m_loadingTime > 2 && !m_isExitTaskStarted)
	{
		DebugDraw::GetInstance()->DebugRenderLogf("EXITING LOADING SCREEN", "");
		((SceneLevel1*)m_level1Scene)->InitScene();
		ExitScene();
	}
	if (isExitingScene(deltaTime))
	{
		UpdateExitingTime(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Render loading scene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLoading::Render()
{
	Camera::SetCurrentCamera(Camera::GetUICamera());
	Renderer::GetInstance()->BeginFrame();
	float deltaSecond = static_cast<float>(Clock::g_theMasterClock->frame.m_seconds);
	deltaSecond = ClampFloat(deltaSecond, 0, 0.016f);
	
	float width = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float height = static_cast<float>(Windows::GetInstance()->GetDimensions().y);

	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(material);
	Renderer::GetInstance()->SetUniform("MODELCOLOR", Rgba::WHITE.GetAsFloats());
	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(width / 2, height / 2,0),Vector3::RIGHT,Vector3::UP, "LOADING..", 20, Rgba::WHITE);
	delete material;
/*
	Material *material1 = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(material1);
	Rgba color;
	color.SetAsFloats(1, 1, 1, 0.2);
	Renderer::GetInstance()->DrawAABB(AABB2(Vector2(0, 0), Vector2(width, height)),color );
	delete material1;*/

	if (IsEnteringScene(deltaSecond))
	{
		DoFullScreenFadeIn();
	}
	if (isExitingScene(deltaSecond))
	{
		DoFullScreenFadeOut();
	}
	
}
