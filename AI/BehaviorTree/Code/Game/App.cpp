#include "App.hpp"
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\EngineSystem.hpp"

App::App()
{
	g_theRenderer   = Renderer::GetInstance();
	g_theInput		= InputSystem::GetInstance();
	g_audio			= AudioSystem::GetInstance();
	g_theGame		= Game::GetInstance();

	Clock::g_theMasterClock = new Clock();
	g_theGameClock = new Clock();

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
	EngineSystem::ShutDown();

}

void App::RunFrame()
{
	Clock::g_theMasterClock->BeginFrame();
	Renderer::GetInstance()->BeginFrame();

	//g_audio->BeginFrame();

	Update(MAX_DELTA_VALUE);
	Render();

	//g_audio->EndFrame();	

	Renderer::GetInstance()->EndFrame();
}

void App::Update(float deltaTime)
{
	g_theInput->BeginFrame();
	g_theGame->Update(deltaTime);
	g_theInput->EndFrame();
	
	EngineSystem::Update(MAX_DELTA_VALUE);
	EngineSystem::UpdateProfiler(deltaTime);
}

void App::Render()
{
	g_theGame->Render();
	EngineSystem::Render();
	EngineSystem::RenderProfiler();
}

bool App::IsReadyToQuit()
{
	return isQuitTriggered;
}
