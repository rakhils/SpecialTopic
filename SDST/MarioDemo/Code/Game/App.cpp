#include "App.hpp"
#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine\Core\Time.hpp"
#include <Windows.h>
#include "Engine\Math\MathUtil.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Shader.hpp"
#include "Engine\Debug\DebugDraw.hpp"

App::App()
{
	Clock::g_theMasterClock = new Clock();
	g_theGameClock = new Clock();
	if(g_controlMode)
	{
		g_initialMarioCount = 1;
	}
	g_theRenderer = Renderer::GetInstance();
	g_theInput	  = InputSystem::GetInstance();
	g_audio		  = AudioSystem::GetInstance();
	g_theGame	  = Game::GetInstance();
	
}

App::~App()
{
	delete g_theGame;
	g_theGame = nullptr;

	delete g_audio;
	g_audio = nullptr;

	delete g_theInput;
	g_theInput = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;
}

void App::RunFrame()
{
	Clock::g_theMasterClock->BeginFrame();
	Renderer::GetInstance()->BeginFrame();
	g_theInput->BeginFrame();
	g_audio->BeginFrame();
	float deltaTime = static_cast<float>(Clock::g_theMasterClock->frame.m_seconds);
	deltaTime = ClampFloat(deltaTime, 0.f, MAX_DELTA_VALUE);
	Update(deltaTime);
	Render();
	g_audio->EndFrame();

	g_theInput->EndFrame();
	Renderer::GetInstance()->EndFrame();
}



void App::Update(float deltaTime)
{
	if(InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_ESCAPE))
	{
		isQuitTriggered = true;
	}
	g_theGame->Update(deltaTime);
	DebugDraw::Update();
}

void App::Render()
{
	g_theGame->Render();
	DebugDraw::Render();
	if (DevConsole::GetInstance()->IsDevConsoleOpen())
	{
		DevConsole::GetInstance()->Render();
	}
}

bool App::IsReadyToQuit()
{
	return isQuitTriggered;
}

