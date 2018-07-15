#include "Engine/Input/InputSystem.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/Windows.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/DevConsole/Profiler/ProfilerManager.hpp"
#include "Engine/Logger/LogManager.hpp"
#include "Game/GamePlay/Scenes/SceneMainMenu.hpp"
#include "Game/GameCommon.hpp"
// CONSTRUCTOR
SceneMainMenu::SceneMainMenu()
{
	
}

SceneMainMenu::SceneMainMenu(std::string str) : Scene(str)
{
	m_forwardRenderingPath = new ForwardRenderingPath();
	AddCamera(Camera::GetUICamera());

	/*Renderable *startTextRenderable = new Renderable();
	startTextRenderable->m_name = "START";
	startTextRenderable->m_modelMatrix.SetIdentity();
	float width = Windows::GetInstance()->GetDimensions().x;
	float height = Windows::GetInstance()->GetDimensions().y;
	startTextRenderable->m_modelMatrix.Translate3D(Vector3(width / 2.0, height / 2.0, 0.f));
	startTextRenderable->m_sortOrder = 1;
	startTextRenderable->SetMaterial(Material::CreateOrGetMaterial("default"));


	Renderable *quitTextRenderable = new Renderable();
	quitTextRenderable->m_name = "QUIT";
	quitTextRenderable->m_modelMatrix.SetIdentity();
	quitTextRenderable->m_modelMatrix.Translate3D(Vector3(width / 2.0, height / 2.0 - 60, 0.f));
	quitTextRenderable->m_sortOrder = 1;
	quitTextRenderable->SetMaterial(Material::CreateOrGetMaterial("default"));

	AddRenderable(startTextRenderable);
	AddRenderable(quitTextRenderable);*/
}

// DESTRUCTOR
SceneMainMenu::~SceneMainMenu()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneMainMenu::Update(float deltaTime)
{
	PROFILE_LOG_SCOPE("SceneMainMenu::Update");
	if (IsEnteringScene(deltaTime))
	{
		UpdateEnteringTime(deltaTime);
	}
	if (isExitingScene(deltaTime))
	{
		UpdateExitingTime(deltaTime);
	}
	LogManager::GetInstance()->LogTaggedPrintf("test", "TEST = %f", static_cast<float>(Clock::g_theMasterClock->total.m_seconds));
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_UP_ARROW))
	{
		m_cursorIndex--;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_DOWN_ARROW))
	{
		m_cursorIndex++;
	}
	m_cursorIndex = LoopValue(m_cursorIndex, 0, 1);
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_ENTER))
	{
		if(m_cursorIndex == 0)
		{
			ExitScene();
			return;
		}
		g_isQuitting = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneMainMenu::Render()
{
	Camera::SetCurrentCamera(Camera::GetUICamera());
	Renderer::GetInstance()->BeginFrame();
	float deltaSecond = static_cast<float>(Clock::g_theMasterClock->frame.m_seconds);
	deltaSecond = ClampFloat(deltaSecond, 0, 0.016f);
	float width = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float height = static_cast<float>(Windows::GetInstance()->GetDimensions().y);
	Vector2 menuSelection(width / 2 - 100, height / 2);
	float   fontSize = 25;
	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(material);
	Renderer::GetInstance()->SetUniform("MODELCOLOR", Rgba::WHITE.GetAsFloats());


	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(menuSelection,0.f),Vector3::RIGHT,Vector3::UP, "START GAME", 20, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(menuSelection.x,menuSelection.y - 4*fontSize, 0), Vector3::RIGHT, Vector3::UP, "QUIT", 20, Rgba::WHITE);
	delete material;
	
	Material *material1 = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(material1);
	Vector2  selectionPosition(menuSelection.x - 50, menuSelection.y + m_cursorIndex*-4*fontSize);
	Renderer::GetInstance()->DrawRectangle(AABB2(selectionPosition, 20,20));
//	Renderer::GetInstance()->DrawRectangle(AABB2(selectionPosition, Vector2(selectionPosition.x + 200,selectionPosition.y + 2*fontSize)));
	//Renderer::GetInstance()->DrawRectangle(AABB2(Vector2(0,0),200,200));

	delete material1;

	if (IsEnteringScene(deltaSecond))
	{
		DoFullScreenFadeIn();
	}
	if (isExitingScene(deltaSecond))
	{
		DoFullScreenFadeOut();
	}
}
