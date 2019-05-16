#include "App.hpp"
#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine\Core\Time.hpp"
#include <Windows.h>
#include "Engine\Math\MathUtil.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Shader.hpp"

App::App()
{
	g_theRenderer = Renderer::GetInstance();
	g_theInput = new InputSystem();
	g_theGame = new Game();

	Clock::g_theMasterClock = new Clock();
	g_theGameClock = new Clock();
}

App::~App()
{
	delete g_theGame;
	g_theGame = nullptr;

	delete g_theInput;
	g_theInput = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;

}

void App::RunFrame()
{
	Clock::g_theMasterClock->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theInput->BeginFrame();

	float deltaTime = static_cast<float>(Clock::g_theMasterClock->GetMasterDeltaTime());
	Update(deltaTime);
	Render();

	g_theInput->EndFrame();
	g_theRenderer->EndFrame();
}



void App::Update(float deltaTime)
{
	g_theGame->Update(deltaTime);
}

void App::Render()
{
	g_theGame->Render();
	if (DevConsole::GetInstance()->IsDevConsoleOpen())
	{
		DevConsole::GetInstance()->Render();
	}
}

bool App::IsReadyToQuit()
{
	return isQuitTriggered;
}

