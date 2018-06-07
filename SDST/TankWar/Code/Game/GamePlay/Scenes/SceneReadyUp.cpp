#include "Engine/Input/InputSystem.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/Windows.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Logger/LogManager.hpp"

#include "Game/GamePlay/Scenes/SceneReadyUp.hpp"
// CONSTRUCTOR
SceneReadyUp::SceneReadyUp()
{
	
}

SceneReadyUp::SceneReadyUp(std::string str) : Scene(str)
{
	m_forwardRenderingPath = new ForwardRenderingPath();
	AddCamera(Camera::GetUICamera());

	/*Renderable *startTextRenderable = new Renderable();
	startTextRenderable->m_name = "READYUP";
	startTextRenderable->m_modelMatrix.SetIdentity();
	float width = Windows::GetInstance()->GetDimensions().x;
	float height = Windows::GetInstance()->GetDimensions().y;
	startTextRenderable->m_modelMatrix.Translate3D(Vector3(width / 2.0, height / 2.0, 0.f));
	startTextRenderable->m_sortOrder = 1;
	startTextRenderable->SetMaterial(Material::CreateOrGetMaterial("default"));

	AddRenderable(startTextRenderable);*/
}

// DESTRUCTOR
SceneReadyUp::~SceneReadyUp()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Update ready up scene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneReadyUp::Update(float deltaTime)
{ 
	if (IsEnteringScene(deltaTime))
	{
		UpdateEnteringTime(deltaTime);
		if (m_isEntryTaskComplete)
		{
		}
	}
	if (m_isEntryTaskComplete)
	{
		m_loadingTime += deltaTime;
	}
	if (m_loadingTime > 2 && !m_isExitTaskStarted)
	{
		ExitScene();
	}
	if (isExitingScene(deltaTime))
	{
		UpdateExitingTime(deltaTime);
		if(m_isExitTaskComplete)
		{
			LogManager::PushLog("levelstart.txt", "SCENE READY FINAL");
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Render scene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneReadyUp::Render()
{
	Camera::SetCurrentCamera(Camera::GetUICamera());
	Renderer::GetInstance()->BeginFrame();

	float deltaSecond = static_cast<float>(Clock::g_theMasterClock->frame.m_seconds);
	deltaSecond = ClampFloat(deltaSecond, 0, 0.016f);
	float width  = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float height = static_cast<float>(Windows::GetInstance()->GetDimensions().y);
	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(material);
	Renderer::GetInstance()->SetUniform("MODELCOLOR", Rgba::WHITE.GetAsFloats());
	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(width / 2, height / 2, 0), Vector3::RIGHT, Vector3::UP, "STARTING GAME", 20, Rgba::WHITE);
	delete material;
	if (IsEnteringScene(deltaSecond))
	{
		DoFullScreenFadeIn();
		return;
	}
	if (isExitingScene(deltaSecond))
	{
		DoFullScreenFadeOut();
		return;
	}
}
