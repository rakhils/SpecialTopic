#pragma once
#include "Game.hpp"
#include "Engine/Net/UDP/UDPTest.hpp"

class NetSession;
class App
{
public:
	//UDPTest m_udp;
	NetSession *m_netSession;
	bool isQuitTriggered = false;
	Game *g_theGame;
	float MAX_DELTA_VALUE = 0.01616f;
	App::App();
	App::~App();
	void App::RunFrame();
	void App::Update(float deltaTime);
	void App::Render();
	bool IsReadyToQuit();
	
};
