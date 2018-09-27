#include <iostream>
#include <conio.h>
#include "App.hpp"
#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine\Core\Time.hpp"
#include "Engine\Math\MathUtil.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Shader.hpp"
#include "Engine\EngineSystem.hpp"
#include "Engine\DevConsole\Profiler\ProfilerManager.hpp"
#include "Engine\Core\Endianness.hpp"
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Net\NetSession.hpp"

App::App()
{
	g_theRenderer   = Renderer::GetInstance();
	g_theInput		= InputSystem::GetInstance();
	g_audio			= AudioSystem::GetInstance();
	g_theGame		= new Game();

	Clock::g_theMasterClock = new Clock();
	g_theGameClock = new Clock();


	Net::StartUp();
	m_netSession = NetSession::GetInstance();
	//m_netSession->Bind(10084);
	/*m_udp.start(10088);*/


	/*NetAddress *addr = new NetAddress();
	sockaddr_storage out;
	int out_addrlen;



	NetAddress::GetRemoteAddress(addr, (sockaddr*)&out,&out_addrlen, "10.8.128.15", "10084");
	std::string str = "hello";
	char const *chararr = str.c_str();*/

	//m_udp.send_to(*addr, chararr, str.length());
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
	Endianness::GetPlatformEndianness();

	ProfilerManager::MarkFrame();
	ProfilerManager::PushProfiler("App::Runframe");
	Clock::g_theMasterClock->BeginFrame();
	Renderer::GetInstance()->BeginFrame();

	g_theInput->BeginFrame();
	//g_audio->BeginFrame();

	Update(MAX_DELTA_VALUE);
	Render();

	//g_audio->EndFrame();
	g_theInput->EndFrame();

	Renderer::GetInstance()->EndFrame();
	ProfilerManager::PoPProfiler();
}

void App::Update(float deltaTime)
{
	ProfilerManager::PushProfiler("App::Update");
	g_theGame->Update(deltaTime);
	m_netSession->Update(deltaTime);
	ProfilerManager::PoPProfiler();
	EngineSystem::Update(MAX_DELTA_VALUE);
	EngineSystem::UpdateProfiler(deltaTime);
	//m_udp.Update();
}

void App::Render()
{
	ProfilerManager::PushProfiler("App::Render");
	g_theGame->Render();
	ProfilerManager::PoPProfiler();
	EngineSystem::Render();
	EngineSystem::RenderProfiler();
}

bool App::IsReadyToQuit()
{
	return isQuitTriggered;
}
