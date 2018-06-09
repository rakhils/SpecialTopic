#pragma once
#include "Game.hpp"

class App
{
public:
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
